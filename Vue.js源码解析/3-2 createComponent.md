# createComponent

关键词：组件 Vnode 创建的过程

逻辑：

## src/core/vdom/create-element.js文件解析

此时传入的是一个组件对象，`object`类型，所以要执行`vnode = createComponent(tag, data, context, children)`代码。

## src/core/vdom/create-component.js文件解析

第一个参数支持传入组件类型的类，也可以是个函数，也可以是个对象（在本例子中是`Object`对象）。第二个是 VNode 相关的 data 。第三个参数是上下文，也是当前 vm 的实例。第四个参数是组件的子的 VNode 。

`baseCtor`是大 Vue （`ƒ Vue(options)`），如果传入的`Ctor`是个对象的话，就把它转换成构造器`ƒ VueComponent(options)`。在构造器的属性中，其中`components`是引入的组件，name是`app`，`render`是`ƒ ()`。

### src/core/global-api/extend.js文件解析

传入对象返回函数，函数就是构造函数。`this`不是 vm ，是指向大 Vue ，因为是静态的方法，调用`Vue.extend`所以`this`指向 Vue 。`Super`是 Vue 。`SuperId`就是 Vue 的`cid`，`cachedCtors`是对对象`extendOptions`添加了`_Ctor`属性，属性先定义为空对象，实际上是做一层缓存的优化。

`name`是组件`name`，然后进行校验，是以字母开头、连接符结尾去校验，不满足会报错。如果传入的组件名称是个`BuiltInTag`，也就是内置的一个`html`标签，有冲突的情况下也会报错（这个比较常见）。

> ^匹配开始；  $匹配结束。
> *是一个限定符，用来修饰前一个字符或分组，限定匹配重复的数量为任意数量。
> [\w-] 匹配任意单词字符[a-zA-Z0-9]、下划线或 '-' 。\w后的 '-' 用来弥补 \w 不能匹配 '-' 的局限。（匹配的是字符）

定义了一个子的构造函数`Sub`，调用了`this._init()`方法，就是把子的构造器的原型指向父的原型。目的是让`Sub`有跟`Vue`一样的能力。最后对于这个`Sub`构造函数做了缓存，避免多次执行`Vue.extend`的时候对同一个子组件重复构造。`cached`第一次肯定是没有的。

## src/core/vdom/create-component.js文件解析

然后判断`Ctor`是否是个函数，不是则报错。

`installComponentHooks(data)`安装一些组件的钩子。遍历`hooksToMerge`。`componentVNodeHooks`是每个组件都会有的 hooks ，例如`init`、`prepatch`、`insert`、`destroy`。整个`installComponentHooks`的过程就是把`componentVNodeHooks`的钩子函数合并到`data.hook`中。在合并过程中，如果某个时机的钩子已经存在`data.hook`中，那么通过执行`mergeHook`函数做合并，这个逻辑很简单，就是在最终执行的时候，依次执行这两个钩子函数即可。

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
3、生成组件 vnode 。