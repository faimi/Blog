# createElement

关键词：

逻辑：回到`mountComponent`函数的过程，至此已经知道`vm._render`是如何创建了一个`VNode`，接下来就是要把这个 VNode 渲染成一个真实的`DOM`并渲染出来，这个过程是通过`vm._update`完成的，接下来分析一下这个过程。

Vue.js 利用`createElement`方法创建`VNode`，它定义在`src/core/vdom/create-element.js`中：

## src/core/vdom/create-element.js文件解析

### createElement()

`createElement`的定义支持六个参数，第一个参数`context`是`vm`实例，第二个`tag`是`VNode`标签（`tag: "div"`），第三个`data`是跟`VNode`相关的数据，第四个`children`是`VNode`的子节点（`children: [VNode]`），有`children`才能构造成`VNode tree`，可以完美映射到`DOM Tree`。

进行参数重载，检测参数，是对参数个数不一致的处理。即没有`data`，传入的是`children`，就会把参数往后移动。

对`alwaysNormalize`进行判断，然后为`normalizationType`赋值常变量。

`createElement`方法实际上是对`_createElement`方法的封装，它允许传入的参数更加灵活，在处理这些参数后，调用真正创建 `VNode`的函数`_createElement`。

```javascript
// src/core/vdom/create-element.js
export function createElement (
  context: Component,
  tag: any,
  data: any,
  children: any,
  normalizationType: any,
  alwaysNormalize: boolean
): VNode | Array<VNode> {
  if (Array.isArray(data) || isPrimitive(data)) {
    normalizationType = children
    children = data
    data = undefined
  }
  if (isTrue(alwaysNormalize)) {
    normalizationType = ALWAYS_NORMALIZE
  }
  return _createElement(context, tag, data, children, normalizationType)
}
```

### _createElement()

`_createElement`函数的流程略微有点多，我们接下来主要分析 2 个重点的流程 —— `children`的规范化以及`VNode`的创建。

`_createElement`方法有5个参数，`context`表示`VNode`的上下文环境，它是`Component`类型；`tag`表示标签，它可以是一个字符串，也可以是一个`Component`；`data`表示`VNode`的数据，它是一个`VNodeData`类型，可以在`flow/vnode.js`中找到它的定义，这里先不展开说；`children`表示当前`VNode`的子节点，它是任意类型的，它接下来需要被规范为**标准的`VNode`数组**；`normalizationType`表示子节点规范的类型，类型不同规范的方法也就不一样，它主要是参考`render`函数是**编译生成**的还是**用户手写**的。

`_createElement`对`data`进行校验，`data`不能是响应式的（有`__ob__`属性代表是响应式的），否则报警告“ VNode data 不能是响应式的 ”。然后调用`createEmptyVNode`函数。

`createEmptyVNode`方法定义在`src/core/vdom/vnode.js`文件中，即简单创建`VNode`实例，什么参数都不传，可以理解为是一个注释节点。

```javascript
// src/core/vdom/vnode.js
export const createEmptyVNode = (text: string = '') => {
  const node = new VNode()
  node.text = text
  node.isComment = true
  return node
}
```

判断`data`和`data.is`，如果`component :is`不是一个真值，也是返回一个注释节点。

对`data`参数，例如`key`不是基础类型则报错。

#### `children`的规范化

对`children`做`normalizeChildren`。当手写render函数时，对第三个参数传了`this.message`，那是一个普通的值，但是实际上`children`应该是个数组，而且每个数组都是`VNode`。`normalizeChildren`和`simpleNormalizeChildren`函数来自`src/core/vdom/helpers/normalize-children.js`文件。

（1）`simpleNormalizeChildren`

`simpleNormalizeChildren`对`children`进行了一层遍历。`children`是个类数组，遍历发现如果有元素是数组，就调用`Array.prototype.concat.apply()`方法把`children`拍平（只拍一次），就是让嵌套数组成为一维数组（是因为存在`functional component`函数式组件返回的是一个数组而不是一个根节点）。最终的期望就是`children`是个一维数组，每个都是一个`VNode`。

- Array.isArray()：判断传递的值是否是一个 Array 。如果对象是 Array ，则返回 true ，否则为 false 。
- 数组降维

  ```javascript
  let children = [1, 2, [3, [4, 5, 6], 7], 8, [9, 10]];
  function simpleNormalizeChildren(children) {
   return Array.prototype.concat.apply([], children);
  }
  console.log(simpleNormalizeChildren(children)) // [1, 2, 3, [4, 5, 6], 7, 8, 9, 10]
  ```

（2）`normalizeChildren`

`normalizeChildren`最终目标也是返回一个一维的数组，每个都是`VNode`。

首先判断是否是个基础类型，是的话直接返回一维数组，数组长度为1，`createTextVNode`实例化一个`VNode`，前三个参数是`undefined`，第四个参数是个`string`，是返回一个文本`VNode`；不是基础类型则判断是否是`Array`类型，是的话调用`normalizeArrayChildren`方法，否则返回`undefined`。

```javascript
export function normalizeChildren (children: any): ?Array<VNode> {
  return isPrimitive(children)
    ? [createTextVNode(children)]
    : Array.isArray(children)
      ? normalizeArrayChildren(children)
      : undefined
}
```

（3）`normalizeArrayChildren`

`normalizeArrayChildren`返回的是`res`数组。遍历`children`，如果`children[i]`是`Array`数组（可能多层嵌套，例如编译`slot`、`v-for`的时候会产生嵌套数组的情况），递归调用`normalizeArrayChildren`，做个优化（如果最后一个节点和下次第一个节点都是文本，则把这两个合并在一起），再做一层`push`；如果是基础类型，判断是否是文本节点，是的话则通过`createTextVNode`方法转换成`VNode`类型，不是的话直接`push`；如果是`VNode`，例如`v-for`，如果`children`是一个列表并且列表还存在嵌套的情况，则根据`nestedIndex`去更新它的`key`。最终返回`res`。
`normalizeArrayChildren`主要是递归和合并。经过对`children`的规范化，`children`变成了一个类型为`VNode`的`Array`。

```javascript
function normalizeArrayChildren (children: any, nestedIndex?: string): Array<VNode> {
  const res = []
  let i, c, lastIndex, last
  for (i = 0; i < children.length; i++) {
    c = children[i]
    if (isUndef(c) || typeof c === 'boolean') continue
    lastIndex = res.length - 1
    last = res[lastIndex]
    //  nested
    if (Array.isArray(c)) {
      if (c.length > 0) {
        c = normalizeArrayChildren(c, `${nestedIndex || ''}_${i}`)
        // merge adjacent text nodes
        if (isTextNode(c[0]) && isTextNode(last)) {
          res[lastIndex] = createTextVNode(last.text + (c[0]: any).text)
          c.shift()
        }
        res.push.apply(res, c)
      }
    } else if (isPrimitive(c)) {
      if (isTextNode(last)) {
        // merge adjacent text nodes
        // this is necessary for SSR hydration because text nodes are
        // essentially merged when rendered to HTML strings
        res[lastIndex] = createTextVNode(last.text + c)
      } else if (c !== '') {
        // convert primitive to vnode
        res.push(createTextVNode(c))
      }
    } else {
      if (isTextNode(c) && isTextNode(last)) {
        // merge adjacent text nodes
        res[lastIndex] = createTextVNode(last.text + c.text)
      } else {
        // default key for nested array children (likely generated by v-for)
        if (isTrue(children._isVList) &&
          isDef(c.tag) &&
          isUndef(c.key) &&
          isDef(nestedIndex)) {
          c.key = `__vlist${nestedIndex}_${i}__`
        }
        res.push(c)
      }
    }
  }
  return res
}
```

#### VNode 的创建

对`tag`进行判断，是个`string`还是组件。如果是`string`，判断是不是 HTML 原生保留标签。如果是则创建一个普通的保留标签，然后直接创建一个普通`vnode`。`vnode = render.call(vm._renderProxy, vm.$createElement)`函数返回的`vnode`是`createElement(vm, a, b, c, d, true)`的返回值。同时把`vnode`返回给`Vue.prototype._render`。

这里先对`tag`做判断，如果是`string`类型，则接着判断是不是 HTML 原生保留标签，则直接创建一个普通`vnode`，如果是为已注册的组件名，则通过`createComponent`创建一个组件类型的`vnode`，否则创建一个未知的标签的`vnode`。如果是`tag`一个`Component`类型，则直接调用`createComponent`创建一个组件类型的`vnode`节点。对于`createComponent`创建组件类型的`vnode`的过程，本质上它还是返回了一个`vnode`。

```javascript
// src/core/vdom/create-element.js
export function _createElement (
  context: Component,
  tag?: string | Class<Component> | Function | Object,
  data?: VNodeData,
  children?: any,
  normalizationType?: number
): VNode | Array<VNode> {
  if (isDef(data) && isDef((data: any).__ob__)) {
    process.env.NODE_ENV !== 'production' && warn(
      `Avoid using observed data object as vnode data: ${JSON.stringify(data)}\n` +
      'Always create fresh vnode data objects in each render!',
      context
    )
    return createEmptyVNode()
  }
  // object syntax in v-bind
  if (isDef(data) && isDef(data.is)) {
    tag = data.is
  }
  if (!tag) {
    // in case of component :is set to falsy value
    return createEmptyVNode()
  }
  // warn against non-primitive key
  if (process.env.NODE_ENV !== 'production' &&
    isDef(data) && isDef(data.key) && !isPrimitive(data.key)
  ) {
    if (!__WEEX__ || !('@binding' in data.key)) {
      warn(
        'Avoid using non-primitive value as key, ' +
        'use string/number value instead.',
        context
      )
    }
  }
  // support single function children as default scoped slot
  if (Array.isArray(children) &&
    typeof children[0] === 'function'
  ) {
    data = data || {}
    data.scopedSlots = { default: children[0] }
    children.length = 0
  }
  if (normalizationType === ALWAYS_NORMALIZE) {
    children = normalizeChildren(children)
  } else if (normalizationType === SIMPLE_NORMALIZE) {
    children = simpleNormalizeChildren(children)
  }
  let vnode, ns
  if (typeof tag === 'string') {
    let Ctor
    ns = (context.$vnode && context.$vnode.ns) || config.getTagNamespace(tag)
    if (config.isReservedTag(tag)) {
      // platform built-in elements
      if (process.env.NODE_ENV !== 'production' && isDef(data) && isDef(data.nativeOn) && data.tag !== 'component') {
        warn(
          `The .native modifier for v-on is only valid on components but it was used on <${tag}>.`,
          context
        )
      }
      vnode = new VNode(
        config.parsePlatformTagName(tag), data, children,
        undefined, undefined, context
      )
    } else if ((!data || !data.pre) && isDef(Ctor = resolveAsset(context.$options, 'components', tag))) {
      // component
      vnode = createComponent(Ctor, data, context, children, tag)
    } else {
      // unknown or unlisted namespaced elements
      // check at runtime because it may get assigned a namespace when its
      // parent normalizes children
      vnode = new VNode(
        tag, data, children,
        undefined, undefined, context
      )
    }
  } else {
    // direct component options / constructor
    vnode = createComponent(tag, data, context, children)
  }
  if (Array.isArray(vnode)) {
    return vnode
  } else if (isDef(vnode)) {
    if (isDef(ns)) applyNS(vnode, ns)
    if (isDef(data)) registerDeepBindings(data)
    return vnode
  } else {
    return createEmptyVNode()
  }
}
```
