# createComponent

关键词：组件Vnode创建的过程

逻辑：

## src/core/vdom/create-element.js文件解析

此时传入的是一个组件对象，`object`类型，所以要执行`vnode = createComponent(tag, data, context, children)`代码。

## src/core/vdom/create-component.js文件解析

第一个参数支持传入组件类型的类，也可以是个函数，也可以是个对象（在本例子中是`Object`对象）。第二个是 VNode 相关的 data 。第三个参数是上下文，也是当前 vm 的实例。第四个参数是组件的子的 VNode 。

`baseCtor`是大 Vue （`ƒ Vue(options)`），如果传入的`Ctor`是个对象的话，就把它转换成构造器`ƒ VueComponent(options)`。

### src/core/global-api/extend.js文件解析

传入对象返回函数，函数就是构造函数。`this`指向大 Vue ，`SuperId`就是 Vue 的`cid`，`cachedCtors`是对对象`extendOptions`添加了`_Ctor`属性，属性先定义为空对象，实际上是做一层缓存的优化。

`name`是组件`name`，然后进行校验，是以字母开头、连接符结尾去校验

> ^匹配开始；  $匹配结束。
> *是一个限定符，用来修饰前一个字符或分组，限定匹配重复的数量为任意数量。
> [\w-] 匹配任意单词字符[a-zA-Z0-9]、下划线或 '-' 。\w后的 '-' 用来弥补 \w 不能匹配 '-' 的局限。（匹配的是字符）

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