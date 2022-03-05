# render

关键词：`vm.$createElement`、`vm._renderProxy`、`VNode`

逻辑：`vm._render`最终是通过执行`createElement`方法并返回的是`vnode`。

`src/core/instance/lifecycle.js`文件运行的`vm._render()`定义在`src/core/instance/render.js`文件中。

## src/core/instance/render.js文件解析

（1）定义了原型上`_render`的私有方法，返回的是一个`VNode`。从`$options`拿到`render`函数（`render`函数可以用户自己写也可以通过编译生成），利用`render.call()`方法传入`vm._renderProxy`和`vm.$createElement`。`call`的第一个参数是当前上下文，`vm._renderProxy`在生产环境（ production ）下就是`vm`，在开发环境（ development ）中可能是`proxy`对象。`vm.$createElement`在`initRender`函数中定义。

```javascript
// src/core/instance/render.js
Vue.prototype._render = function (): VNode {
  const vm: Component = this
  const { render, _parentVnode } = vm.$options
  if (_parentVnode) {
    vm.$scopedSlots = normalizeScopedSlots(
      _parentVnode.data.scopedSlots,
      vm.$slots,
      vm.$scopedSlots
    )
  }
  // set parent vnode. this allows render functions to have access
  // to the data on the placeholder node.
  vm.$vnode = _parentVnode
  // render self
  let vnode
  try {
    // There's no need to maintain a stack because all render fns are called
    // separately from one another. Nested component's render fns are called
    // when parent component is patched.
    currentRenderingInstance = vm
    vnode = render.call(vm._renderProxy, vm.$createElement)
  } catch (e) {
    handleError(e, vm, `render`)
    // return error render result,
    // or previous vnode to prevent render error causing blank component
    /* istanbul ignore else */
    if (process.env.NODE_ENV !== 'production' && vm.$options.renderError) {
      try {
        vnode = vm.$options.renderError.call(vm._renderProxy, vm.$createElement, e)
      } catch (e) {
        handleError(e, vm, `renderError`)
        vnode = vm._vnode
      }
    } else {
      vnode = vm._vnode
    }
  } finally {
    currentRenderingInstance = null
  }
  // if the returned array contains only a single node, allow it
  if (Array.isArray(vnode) && vnode.length === 1) {
    vnode = vnode[0]
  }
  // return empty vnode in case the render function errored out
  if (!(vnode instanceof VNode)) {
    if (process.env.NODE_ENV !== 'production' && Array.isArray(vnode)) {
      warn(
        'Multiple root nodes returned from render function. Render function ' +
        'should return a single root node.',
        vm
      )
    }
    vnode = createEmptyVNode()
  }
  // set parent
  vnode.parent = _parentVnode
  return vnode
}
```

`initRender`函数在`src/core/instance/init.js`中执行`initRender(vm)`。`initRender`中定义了两个函数：`vm._c`和`vm.$createElement`。这两个函数最终都调用了`createElement`方法，区别是最后一个参数不一样。因为`vm._c`是被编译生成的`render`函数所使用的方法，`vm.$createElement`实际上是给我们手写`render`函数提供了一个创建 VNode 的方法。

```javascript
// src/core/instance/render.js
export function initRender (vm: Component) {
  vm._vnode = null // the root of the child tree
  vm._staticTrees = null // v-once cached trees
  const options = vm.$options
  const parentVnode = vm.$vnode = options._parentVnode // the placeholder node in parent tree
  const renderContext = parentVnode && parentVnode.context
  vm.$slots = resolveSlots(options._renderChildren, renderContext)
  vm.$scopedSlots = emptyObject
  // bind the createElement fn to this instance
  // so that we get proper render context inside it.
  // args order: tag, data, children, normalizationType, alwaysNormalize
  // internal version is used by render functions compiled from templates
  vm._c = (a, b, c, d) => createElement(vm, a, b, c, d, false)
  // normalization is always applied for the public version, used in
  // user-written render functions.
  vm.$createElement = (a, b, c, d) => createElement(vm, a, b, c, d, true)

  // $attrs & $listeners are exposed for easier HOC creation.
  // they need to be reactive so that HOCs using them are always updated
  const parentData = parentVnode && parentVnode.data

  /* istanbul ignore else */
  if (process.env.NODE_ENV !== 'production') {
    defineReactive(vm, '$attrs', parentData && parentData.attrs || emptyObject, () => {
      !isUpdatingChildComponent && warn(`$attrs is readonly.`, vm)
    }, true)
    defineReactive(vm, '$listeners', options._parentListeners || emptyObject, () => {
      !isUpdatingChildComponent && warn(`$listeners is readonly.`, vm)
    }, true)
  } else {
    defineReactive(vm, '$attrs', parentData && parentData.attrs || emptyObject, null, true)
    defineReactive(vm, '$listeners', options._parentListeners || emptyObject, null, true)
  }
}
```

（2）手写`render`函数（使用`$createElement`）

这跟直接在 html 上写是不一样的。他没有一个把从插值变换的过程。之前的写法是在 html 里面定义了一个插值，会先渲染出来，然后在`new Vue`之后执行`$mount`的时候再把它从插值`message`替换成真实的数据。而这是通过纯`render`函数，不用在页面上显示插值，而是通过`render`函数执行完毕之后把`message`替换上去，体验会更好。因为手写`render`函数就不会执行把`template`转换成`render`函数这一步了，挂载的元素（`id="app1"`）实际上会替换掉定义的（`id="app"`）。这就是为什么不能在`body`上做这个事情，因为会把`body`替换。

```html
<div id="app"></div>
```

```javascript
export default {
  data() {
    return {
      message: 'Hello Vue!',
    };
  },
  render(createElement){
    return createElement('div',{
      attrs:{
        id:'app1'
      }
    },this.message)
  },
};
```

（3）`vm._renderProxy`也是发生在`src/core/instance/init.js`中，如果当前是生产环境，就`vm._renderProxy = vm`，开发阶段则`initProxy(vm)`，`initProxy()`的定义在`src/core/instance/proxy.js`文件中。判断`hasProxy`（判断当前浏览器支不支持`proxy`，`proxy`实际上是 ES6 提供的 API ，实际作用就是对对象访问做一个劫持）。因为chrome支持`proxy`，所以会执行`vm._renderProxy = new Proxy(vm, handlers)`，`handlers`在目前条件下指向`hasHandler`。`hasHandler`是个判断，如果我们的元素不在`target`下，`has`为`false`。`isAllowed`是全局的属性和方法。在两个情况都不满足的条件下，执行`warnNonPresent`方法。`warnNonPresent`就是报警告。该警告是报使用了一个未在data、method定义的一个变量。

```javascript
let initProxy

if (process.env.NODE_ENV !== 'production') {
  const allowedGlobals = makeMap(
    'Infinity,undefined,NaN,isFinite,isNaN,' +
    'parseFloat,parseInt,decodeURI,decodeURIComponent,encodeURI,encodeURIComponent,' +
    'Math,Number,Date,Array,Object,Boolean,String,RegExp,Map,Set,JSON,Intl,BigInt,' +
    'require' // for Webpack/Browserify
  )

  const warnNonPresent = (target, key) => {
    warn(
      `Property or method "${key}" is not defined on the instance but ` +
      'referenced during render. Make sure that this property is reactive, ' +
      'either in the data option, or for class-based components, by ' +
      'initializing the property. ' +
      'See: https://vuejs.org/v2/guide/reactivity.html#Declaring-Reactive-Properties.',
      target
    )
  }

  const warnReservedPrefix = (target, key) => {
    warn(
      `Property "${key}" must be accessed with "$data.${key}" because ` +
      'properties starting with "$" or "_" are not proxied in the Vue instance to ' +
      'prevent conflicts with Vue internals. ' +
      'See: https://vuejs.org/v2/api/#data',
      target
    )
  }

  const hasProxy =
    typeof Proxy !== 'undefined' && isNative(Proxy)

  if (hasProxy) {
    const isBuiltInModifier = makeMap('stop,prevent,self,ctrl,shift,alt,meta,exact')
    config.keyCodes = new Proxy(config.keyCodes, {
      set (target, key, value) {
        if (isBuiltInModifier(key)) {
          warn(`Avoid overwriting built-in modifier in config.keyCodes: .${key}`)
          return false
        } else {
          target[key] = value
          return true
        }
      }
    })
  }

  const hasHandler = {
    has (target, key) {
      const has = key in target
      const isAllowed = allowedGlobals(key) ||
        (typeof key === 'string' && key.charAt(0) === '_' && !(key in target.$data))
      if (!has && !isAllowed) {
        if (key in target.$data) warnReservedPrefix(target, key)
        else warnNonPresent(target, key)
      }
      return has || !isAllowed
    }
  }

  const getHandler = {
    get (target, key) {
      if (typeof key === 'string' && !(key in target)) {
        if (key in target.$data) warnReservedPrefix(target, key)
        else warnNonPresent(target, key)
      }
      return target[key]
    }
  }

  initProxy = function initProxy (vm) {
    if (hasProxy) {
      // determine which proxy handler to use
      const options = vm.$options
      const handlers = options.render && options.render._withStripped
        ? getHandler
        : hasHandler
      vm._renderProxy = new Proxy(vm, handlers)
    } else {
      vm._renderProxy = vm
    }
  }
}

export { initProxy }
```

`render`方法实际上就是生成一个`vnode`，出错的话`handleError`会给用户一个接口去处理一些错误，再做一系列降级。再对`vnode`进行判断是不是`VNode`，如果同时是个`Array`，说明模板会有多个根节点，会返回多个`vnode`。`vnode`实际上是`Virtual DOM`的概念。
