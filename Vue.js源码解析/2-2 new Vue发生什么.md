关键词：

# `new Vue`发生了什么

从入口代码开始分析，`new Vue`背后发生了哪些事情。

## 入口代码文件`src/core/instance/index.js`(Vue 定义)

Vue 实际上就是`function`实现的`class`，执行`new Vue`的时候执行了`function`，然后执行`this._init`把`options`传入，`this._init`是一个 Vue 原型上的方法，是在`initMixin`，也就是`src/core/instance/init.js`中定义的。

```javascript
// src/core/instance/index.js
import { initMixin } from './init'
import { stateMixin } from './state'

function Vue (options) {
  if (process.env.NODE_ENV !== 'production' &&
    !(this instanceof Vue)
  ) {
    warn('Vue is a constructor and should be called with the `new` keyword')
  }
  this._init(options)
}

initMixin(Vue)
stateMixin(Vue)

export default Vue
```

## src/core/instance/init.js文件解析

在原型上定义了`_init`方法，也就是说走到了`initMixin(Vue)`时，执行了`_init`方法。

`_init`方法做了很多初始化的工作，例如:
1. 定义`uid`
2. 合并`options`。将传入的`options` merge 到`$options`上，所以可以通过`$options.el`访问到代码中定义的`el`，通过`$options.data`访问到我们定义的`data`。
3. 初始化函数（生命周期、事件中心、`render`、`state`），初始化结束后判断`$options`有没有`el`。调用`vm.$mount(vm.$options.el)`进行挂载，在页面上可以看到字符串渲染到页面上。`$mount`方法是整个做挂载的方法（是个重点）。

```javascript
export function initMixin (Vue: Class<Component>) {
  Vue.prototype._init = function (options?: Object) {
    const vm: Component = this
    // a uid
    vm._uid = uid++

    let startTag, endTag
    /* istanbul ignore if */
    if (process.env.NODE_ENV !== 'production' && config.performance && mark) {
      startTag = `vue-perf-start:${vm._uid}`
      endTag = `vue-perf-end:${vm._uid}`
      mark(startTag)
    }

    // a flag to avoid this being observed
    vm._isVue = true
    // merge options
    if (options && options._isComponent) {
      // optimize internal component instantiation
      // since dynamic options merging is pretty slow, and none of the
      // internal component options needs special treatment.
      initInternalComponent(vm, options)
    } else {
      vm.$options = mergeOptions(
        resolveConstructorOptions(vm.constructor),
        options || {},
        vm
      )
    }
    /* istanbul ignore else */
    if (process.env.NODE_ENV !== 'production') {
      initProxy(vm)
    } else {
      vm._renderProxy = vm
    }
    // expose real self
    vm._self = vm
    initLifecycle(vm)
    initEvents(vm)
    initRender(vm)
    callHook(vm, 'beforeCreate')
    initInjections(vm) // resolve injections before data/props
    initState(vm)
    initProvide(vm) // resolve provide after data/props
    callHook(vm, 'created')

    /* istanbul ignore if */
    if (process.env.NODE_ENV !== 'production' && config.performance && mark) {
      vm._name = formatComponentName(vm, false)
      mark(endTag)
      measure(`vue ${vm._name} init`, startTag, endTag)
    }

    if (vm.$options.el) {
      vm.$mount(vm.$options.el)
    }
  }
}
```

## src/core/instance/state.js文件解析

问：为什么在`mounted(){console.log(this.message)}`可以访问到`message`？
答：（1）初始化时有一个`initState`，这个函数是定义在`src/core/instance/state.js`中。在`initState()`中，判断`options`，如果定义`props`则初始化`props`，定义了`methods`初始化`methods`，定义了`data`初始化`data`。

```javascript
export function initState (vm: Component) {
  vm._watchers = []
  const opts = vm.$options
  if (opts.props) initProps(vm, opts.props)
  if (opts.methods) initMethods(vm, opts.methods)
  if (opts.data) {
    initData(vm)
  } else {
    observe(vm._data = {}, true /* asRootData */)
  }
  if (opts.computed) initComputed(vm, opts.computed)
  if (opts.watch && opts.watch !== nativeWatch) {
    initWatch(vm, opts.watch)
  }
}
```

（2）在`initData`中，从`$options.data`中拿到`data`，就是我们定义的`data(){return {message:'Hello Vue!'}}`。然后判断`data`是不是一个`function`（正常`data`是一个函数而不是对象），是函数则调用`getData`。`getData`中调用`call`方法，返回对象，赋值给`vm._data`和`data`，如果不是对象就会报警告。然后拿到对象的`key`、`props`、`methods`并进行对比，判断是否重名。为什么不能重名？因为他们最终都挂载到vm上，也就是当前实例上。实现是用```proxy(vm,`_data`,key)```实现。

```javascript
function initData (vm: Component) {
  let data = vm.$options.data
  data = vm._data = typeof data === 'function'
    ? getData(data, vm)
    : data || {}
  if (!isPlainObject(data)) {
    data = {}
    process.env.NODE_ENV !== 'production' && warn(
      'data functions should return an object:/n' +
      'https://vuejs.org/v2/guide/components.html#data-Must-Be-a-Function',
      vm
    )
  }
  // proxy data on instance
  const keys = Object.keys(data)
  const props = vm.$options.props
  const methods = vm.$options.methods
  let i = keys.length
  while (i--) {
    const key = keys[i]
    if (process.env.NODE_ENV !== 'production') {
      if (methods && hasOwn(methods, key)) {
        warn(
          `Method "${key}" has already been defined as a data property.`,
          vm
        )
      }
    }
    if (props && hasOwn(props, key)) {
      process.env.NODE_ENV !== 'production' && warn(
        `The data property "${key}" is already declared as a prop. ` +
        `Use prop default value instead.`,
        vm
      )
    } else if (!isReserved(key)) {
      proxy(vm, `_data`, key)
    }
  }
  // observe data
  observe(data, true /* asRootData */)
}

export function getData (data: Function, vm: Component): any {
  // #7573 disable dep collection when invoking data getters
  pushTarget()
  try {
    return data.call(vm, vm)
  } catch (e) {
    handleError(e, vm, `data()`)
    return {}
  } finally {
    popTarget()
  }
}
```

（3）`proxy`通过`sharedPropertyDefinition`对象定义了`get`和`set`两个函数，运行`Object.defineProperty(target, key, sharedPropertyDefinition)`方法代理了`target`和`key`，就是对`target`和`key`做了一层访问`get`和`set`，`target`就是`vm`，`vm.key`的`get`会执行`return this[sourceKey][key]`。会执行`sourceKey`就是`_data`，所以访问`vm.msg`实际上就会访问`vm._data.msg`

```javascript
const sharedPropertyDefinition = {
  enumerable: true,
  configurable: true,
  get: noop,
  set: noop
}

export function proxy (target: Object, sourceKey: string, key: string) {
  sharedPropertyDefinition.get = function proxyGetter () {
    return this[sourceKey][key]
  }
  sharedPropertyDefinition.set = function proxySetter (val) {
    this[sourceKey][key] = val
  }
  Object.defineProperty(target, key, sharedPropertyDefinition)
}
```

## 在项目中如何调试vue

在node_modules下找到vue，package.json中的`module`指向的入口可能不是真的入口，因为在新建webpack项目时如果选择了Runtime + Compiler版本，则真正的入口是在build文件下的webpack.base.conf.js下的`alias`的`vue`

可以在入口文件下打`debugger`，在Sources下进行debugger，第一个按钮是从一个断点跳到另外一个断点，第二个会跳过函数，第三个单步执行，可以看函数逻辑