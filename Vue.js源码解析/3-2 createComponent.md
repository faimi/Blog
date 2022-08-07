# createComponent

关键词：组件 Vnode 创建的过程

逻辑：

## src/core/vdom/create-element.js文件解析

`_createElement`方法主要的函数是`normalizeChildren`方法，然后通过判断`tag`类型去创建不同的 vnode 。这次的 `tag`是组件对象，是 object 类型，也就是 vnode 生成用的是`vnode = createComponent(tag, data, context, children)`。`createComponent`方法定义在 src/core/vdom/create-component.js 文件中。

createComponent 针对组件渲染这个 case 主要就 3 个关键步骤：

构造子类构造函数，安装组件钩子函数和实例化 vnode。

## 构造子类构造函数

第一个参数支持传入组件类型的类，也可以是个函数，也可以是个对象（在本例子中是`Object`对象）。第二个是 VNode 相关的 data 。第三个参数是上下文，也是当前 vm 的实例。第四个参数是组件的子的 VNode 。

`baseCtor`是大 Vue （`ƒ Vue(options)`）。如果传入的`Ctor`是个对象的话，就把它转换成构造器`ƒ VueComponent(options)`。在构造器的属性中，其中`components`是引入的组件，name是`app`，`render`是`ƒ ()`。

### src/core/global-api/extend.js文件解析

传入对象返回函数，函数就是构造函数。`this`不是 vm ，是指向大 Vue ，因为是静态的方法，调用`Vue.extend`所以`this`指向 Vue 。`Super`是 Vue 。`SuperId`就是 Vue 的`cid`，`cachedCtors`是对对象`extendOptions`添加了`_Ctor`属性，属性先定义为空对象，实际上是做一层缓存的优化。

`name`是组件`name`，然后进行校验，是以字母开头、连接符结尾去校验，不满足会报错。如果传入的组件名称是个`BuiltInTag`，也就是内置的一个`html`标签，有冲突的情况下也会报错（这个比较常见）。

> ^匹配开始；  $匹配结束。
> *是一个限定符，用来修饰前一个字符或分组，限定匹配重复的数量为任意数量。
> [\w-] 匹配任意单词字符[a-zA-Z0-9]、下划线或 '-' 。\w后的 '-' 用来弥补 \w 不能匹配 '-' 的局限。（匹配的是字符）

```javascript
export function validateComponentName (name: string) {
  if (!new RegExp(`^[a-zA-Z][\\-\\.0-9_${unicodeRegExp.source}]*$`).test(name)) {
    warn(
      'Invalid component name: "' + name + '". Component names ' +
      'should conform to valid custom element name in html5 specification.'
    )
  }
  if (isBuiltInTag(name) || config.isReservedTag(name)) {
    warn(
      'Do not use built-in or reserved HTML elements as component ' +
      'id: ' + name
    )
  }
}
```

定义了一个子的构造函数`Sub`，也调用了`this._init()`方法，就是把子的构造器的原型指向父的原型，也就是原型继承。然后进行一系列的优化、实例化。目的是让`Sub`有跟`Vue`一样的能力。最后对于这个`Sub`构造函数做了缓存，避免多次执行`Vue.extend`的时候对同一个子组件重复构造。`cached`第一次肯定是没有的。

`Vue.extend`的作用就是构造一个`Vue`的子类，它使用一种非常经典的原型继承的方式把一个纯对象转换一个继承于`Vue`的构造器`Sub`并返回，然后对`Sub`这个对象本身扩展了一些属性，如扩展`options`、添加全局`API`等；并且对配置中的`props`和`computed`做了初始化工作；最后对于这个`Sub`构造函数做了缓存，避免多次执行`Vue.extend`的时候对同一个子组件重复构造。

```javascript
Vue.extend = function (extendOptions: Object): Function {
  extendOptions = extendOptions || {}
  const Super = this
  const SuperId = Super.cid
  const cachedCtors = extendOptions._Ctor || (extendOptions._Ctor = {})
  if (cachedCtors[SuperId]) {
    return cachedCtors[SuperId]
  }
  const name = extendOptions.name || Super.options.name
  if (process.env.NODE_ENV !== 'production' && name) {
    validateComponentName(name)
  }
  const Sub = function VueComponent (options) {
    this._init(options)
  }
  Sub.prototype = Object.create(Super.prototype)
  Sub.prototype.constructor = Sub
  Sub.cid = cid++
  Sub.options = mergeOptions(
    Super.options,
    extendOptions
  )
  Sub['super'] = Super
  // For props and computed properties, we define the proxy getters on
  // the Vue instances at extension time, on the extended prototype. This
  // avoids Object.defineProperty calls for each instance created.
  if (Sub.options.props) {
    initProps(Sub)
  }
  if (Sub.options.computed) {
    initComputed(Sub)
  }
  // allow further extension/mixin/plugin usage
  Sub.extend = Super.extend
  Sub.mixin = Super.mixin
  Sub.use = Super.use
  // create asset registers, so extended classes
  // can have their private assets too.
  ASSET_TYPES.forEach(function (type) {
    Sub[type] = Super[type]
  })
  // enable recursive self-lookup
  if (name) {
    Sub.options.components[name] = Sub
  }
  // keep a reference to the super options at extension time.
  // later at instantiation we can check if Super's options have
  // been updated.
  Sub.superOptions = Super.options
  Sub.extendOptions = extendOptions
  Sub.sealedOptions = extend({}, Sub.options)
  // cache constructor
  cachedCtors[SuperId] = Sub
  return Sub
}
```

## 安装组件钩子函数

然后判断`Ctor`是否是个函数，不是则报错。

`installComponentHooks(data)`安装一些组件的钩子。遍历`hooksToMerge`。`componentVNodeHooks`是每个组件都会有的 hooks ，例如`init`、`prepatch`、`insert`、`destroy`。整个`installComponentHooks`的过程就是把`componentVNodeHooks`的钩子函数 merge 到`data.hook`中。在合并过程中，如果某个时机的钩子已经存在`data.hook`中，那么通过执行`mergeHook`函数做合并，这个逻辑很简单，就是在最终执行的时候，依次执行这两个钩子函数即可。

```javascript
// src/core/vdom/create-component.js
const componentVNodeHooks = {
  init (vnode: VNodeWithData, hydrating: boolean): ?boolean {
    if (
      vnode.componentInstance &&
      !vnode.componentInstance._isDestroyed &&
      vnode.data.keepAlive
    ) {
      // kept-alive components, treat as a patch
      const mountedNode: any = vnode // work around flow
      componentVNodeHooks.prepatch(mountedNode, mountedNode)
    } else {
      const child = vnode.componentInstance = createComponentInstanceForVnode(
        vnode,
        activeInstance
      )
      child.$mount(hydrating ? vnode.elm : undefined, hydrating)
    }
  },

  prepatch (oldVnode: MountedComponentVNode, vnode: MountedComponentVNode) {
    const options = vnode.componentOptions
    const child = vnode.componentInstance = oldVnode.componentInstance
    updateChildComponent(
      child,
      options.propsData, // updated props
      options.listeners, // updated listeners
      vnode, // new parent vnode
      options.children // new children
    )
  },

  insert (vnode: MountedComponentVNode) {
    const { context, componentInstance } = vnode
    if (!componentInstance._isMounted) {
      componentInstance._isMounted = true
      callHook(componentInstance, 'mounted')
    }
    if (vnode.data.keepAlive) {
      if (context._isMounted) {
        // vue-router#1212
        // During updates, a kept-alive component's child components may
        // change, so directly walking the tree here may call activated hooks
        // on incorrect children. Instead we push them into a queue which will
        // be processed after the whole patch process ended.
        queueActivatedComponent(componentInstance)
      } else {
        activateChildComponent(componentInstance, true /* direct */)
      }
    }
  },

  destroy (vnode: MountedComponentVNode) {
    const { componentInstance } = vnode
    if (!componentInstance._isDestroyed) {
      if (!vnode.data.keepAlive) {
        componentInstance.$destroy()
      } else {
        deactivateChildComponent(componentInstance, true /* direct */)
      }
    }
  }
}

const hooksToMerge = Object.keys(componentVNodeHooks)

function installComponentHooks (data: VNodeData) {
  const hooks = data.hook || (data.hook = {})
  for (let i = 0; i < hooksToMerge.length; i++) {
    const key = hooksToMerge[i]
    const existing = hooks[key]
    const toMerge = componentVNodeHooks[key]
    if (existing !== toMerge && !(existing && existing._merged)) {
      hooks[key] = existing ? mergeHook(toMerge, existing) : toMerge
    }
  }
}

function mergeHook (f1: any, f2: any): Function {
  const merged = (a, b) => {
    // flow complains about extra args which is why we use any
    f1(a, b)
    f2(a, b)
  }
  merged._merged = true
  return merged
}
```

## 实例化 vnode

生成 vnode 组件，和 VNode 不一样。标识会加上`vue-component-`（`vue-component-1-app`）。组件 vnode 的`children`一定是个空，`text`和`elm`也是空，但是`componentOptions`里包含了构造器、`props`数据、事件数据、`children`（`{ Ctor, propsData, listeners, tag, children }`）。

```javascript
// src/core/vdom/create-component.js
export function createComponent (
  Ctor: Class<Component> | Function | Object | void,
  data: ?VNodeData,
  context: Component,
  children: ?Array<VNode>,
  tag?: string
): VNode | Array<VNode> | void {
  if (isUndef(Ctor)) {
    return
  }

  const baseCtor = context.$options._base

  // plain options object: turn it into a constructor
  if (isObject(Ctor)) {
    Ctor = baseCtor.extend(Ctor)
  }

  // if at this stage it's not a constructor or an async component factory,
  // reject.
  if (typeof Ctor !== 'function') {
    if (process.env.NODE_ENV !== 'production') {
      warn(`Invalid Component definition: ${String(Ctor)}`, context)
    }
    return
  }

  // async component
  let asyncFactory
  if (isUndef(Ctor.cid)) {
    asyncFactory = Ctor
    Ctor = resolveAsyncComponent(asyncFactory, baseCtor)
    if (Ctor === undefined) {
      // return a placeholder node for async component, which is rendered
      // as a comment node but preserves all the raw information for the node.
      // the information will be used for async server-rendering and hydration.
      return createAsyncPlaceholder(
        asyncFactory,
        data,
        context,
        children,
        tag
      )
    }
  }

  data = data || {}

  // resolve constructor options in case global mixins are applied after
  // component constructor creation
  resolveConstructorOptions(Ctor)

  // transform component v-model data into props & events
  if (isDef(data.model)) {
    transformModel(Ctor.options, data)
  }

  // extract props
  const propsData = extractPropsFromVNodeData(data, Ctor, tag)

  // functional component
  if (isTrue(Ctor.options.functional)) {
    return createFunctionalComponent(Ctor, propsData, data, context, children)
  }

  // extract listeners, since these needs to be treated as
  // child component listeners instead of DOM listeners
  const listeners = data.on
  // replace with listeners with .native modifier
  // so it gets processed during parent component patch.
  data.on = data.nativeOn

  if (isTrue(Ctor.options.abstract)) {
    // abstract components do not keep anything
    // other than props & listeners & slot

    // work around flow
    const slot = data.slot
    data = {}
    if (slot) {
      data.slot = slot
    }
  }

  // install component management hooks onto the placeholder node
  installComponentHooks(data)

  // return a placeholder vnode
  const name = Ctor.options.name || tag
  const vnode = new VNode(
    `vue-component-${Ctor.cid}${name ? `-${name}` : ''}`,
    data, undefined, undefined, undefined, context,
    { Ctor, propsData, listeners, tag, children },
    asyncFactory
  )

  // Weex specific: invoke recycle-list optimized @render function for
  // extracting cell-slot template.
  // https://github.com/Hanks10100/weex-native-directive/tree/master/component
  /* istanbul ignore if */
  if (__WEEX__ && isRecyclableComponent(vnode)) {
    return renderRecyclableComponentTemplate(vnode)
  }

  return vnode
}
```

核心：
1、子组件组件构造器的生成。是基于大 Vue 的。
2、组件 vnode data 都会有 hook ，这些 hook 会 merge 到组件的 VNode Hook 。
3、生成组件 vnode ，例如没有 children ，多了 componentOptions 。