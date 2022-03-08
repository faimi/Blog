# update

关键词：

逻辑：

`Vue`的`_update`是实例的一个私有方法，`_update`方法的作用是把`VNode`渲染成真实的`DOM`。定义在`src/core/instance/lifecycle.js`文件下。`_update`被调用的时机有2个，首次渲染的时候会调用`_update`，把`VNode`映射成真实`DOM`；当改变数据的时候，数据改变也会驱动视图变化，也会调用`_update`方法。

## src/core/instance/lifecycle.js

定义数据分析变量（也就是第二种时机，因此在第一种时机下，变量都为`null`）。因为是`null`所以会进入 initial render 下，调用`vm.__patch__()`方法。`vm.__patch__()`定义在`src/platforms/web/runtime/index.js`文件下。

`inBrowser`是指浏览器环境。因为Vue.js可以跑在服务端，因此在服务端渲染中，没有真实的浏览器`DOM`环境，所以不需要把`VNode`最终转换成`DOM`，因此是一个空函数，而在浏览器端渲染中，它指向了`patch`方法，它的定义在`src/platforms/web/runtime/patch.js`文件夹中。

```javascript
Vue.prototype.__patch__ = inBrowser ? patch : noop
```

`patch`方法调用了`createPatchFunction`方法，返回的是一个函数，传入了`nodeOps`和`modules`。`nodeOps`实际上都是`DOM`的操作，也就是在`web`下面操作`DOM`的一些方法。`modules`是`platformModules`和`baseModules`的`concat`合集。`platformModules`定义了属性、类的钩子函数、 class 等。`createPatchFunction`方法定义在`src/core/vdom/patch.js`文件中。

```javascript
export const patch: Function = createPatchFunction({ nodeOps, modules })
```

拿到`modules`和`nodeOps`，把`hooks`保留到`cbs`对象中，遍历所有模块，拿到所有模块的钩子(hooks)。即初始化定义。

最后返回一个 function `patch`，也就是说调用`vm.__patch__()`方法就是调用这个函数。

【编程技巧】
问：为何 Vue.js 源码绕了这么一大圈，把相关代码分散到各个目录？
答：利用了函数柯里化的技巧，因为`nodeOps`和`modules`都是与平台相关的。因为 Vue.js 是跨端的，可以跑在 web 上也可以跑在 weex 上，在 web 上真实操作 DOM API ， weex 上操作 DOM API ，虽然可以生成同样的 Virtual DOM ，但是真实生成 DOM 的方法是不一样的。`modules`是因为模块在不同生命周期要做的事情、怎么生成这些模块是和 DOM 做配合的，这些东西和平台也都是相关的。`platformModules`就是和平台相关的模块。通过函数柯里化将差异搞定一次性传入，否则就要在`patch`方法里写很多`if else`去判断`web`和`weex`。即将平台差异化的东西在之前就抹平了，`return function patch`函数不用关心真实操作是什么，因为把参数提前传入了，通过这种闭包的技巧实现`nodeOps`和`modules`的持有。接下来调用每次的方法的时候都不用传差异化参数。

```javascript
const hooks = ['create', 'activate', 'update', 'remove', 'destroy']

export function createPatchFunction (backend) {
  let i, j
  const cbs = {}

  const { modules, nodeOps } = backend

  for (i = 0; i < hooks.length; ++i) {
    cbs[hooks[i]] = []
    for (j = 0; j < modules.length; ++j) {
      if (isDef(modules[j][hooks[i]])) {
        cbs[hooks[i]].push(modules[j][hooks[i]])
      }
    }
  }

  return function patch (oldVnode, vnode, hydrating, removeOnly) {
    if (isUndef(vnode)) {
      if (isDef(oldVnode)) invokeDestroyHook(oldVnode)
      return
    }

    let isInitialPatch = false
    const insertedVnodeQueue = []

    if (isUndef(oldVnode)) {
      // empty mount (likely as component), create new root element
      isInitialPatch = true
      createElm(vnode, insertedVnodeQueue)
    } else {
      const isRealElement = isDef(oldVnode.nodeType)
      if (!isRealElement && sameVnode(oldVnode, vnode)) {
        // patch existing root node
        patchVnode(oldVnode, vnode, insertedVnodeQueue, null, null, removeOnly)
      } else {
        if (isRealElement) {
          // mounting to a real element
          // check if this is server-rendered content and if we can perform
          // a successful hydration.
          if (oldVnode.nodeType === 1 && oldVnode.hasAttribute(SSR_ATTR)) {
            oldVnode.removeAttribute(SSR_ATTR)
            hydrating = true
          }
          if (isTrue(hydrating)) {
            if (hydrate(oldVnode, vnode, insertedVnodeQueue)) {
              invokeInsertHook(vnode, insertedVnodeQueue, true)
              return oldVnode
            } else if (process.env.NODE_ENV !== 'production') {
              warn(
                'The client-side rendered virtual DOM tree is not matching ' +
                'server-rendered content. This is likely caused by incorrect ' +
                'HTML markup, for example nesting block-level elements inside ' +
                '<p>, or missing <tbody>. Bailing hydration and performing ' +
                'full client-side render.'
              )
            }
          }
          // either not server-rendered, or hydration failed.
          // create an empty node and replace it
          oldVnode = emptyNodeAt(oldVnode)
        }

        // replacing existing element
        const oldElm = oldVnode.elm
        const parentElm = nodeOps.parentNode(oldElm)

        // create new node
        createElm(
          vnode,
          insertedVnodeQueue,
          // extremely rare edge case: do not insert if old element is in a
          // leaving transition. Only happens when combining transition +
          // keep-alive + HOCs. (#4590)
          oldElm._leaveCb ? null : parentElm,
          nodeOps.nextSibling(oldElm)
        )

        // update parent placeholder node element, recursively
        if (isDef(vnode.parent)) {
          let ancestor = vnode.parent
          const patchable = isPatchable(vnode)
          while (ancestor) {
            for (let i = 0; i < cbs.destroy.length; ++i) {
              cbs.destroy[i](ancestor)
            }
            ancestor.elm = vnode.elm
            if (patchable) {
              for (let i = 0; i < cbs.create.length; ++i) {
                cbs.create[i](emptyNode, ancestor)
              }
              // #6513
              // invoke insert hooks that may have been merged by create hooks.
              // e.g. for directives that uses the "inserted" hook.
              const insert = ancestor.data.hook.insert
              if (insert.merged) {
                // start at index 1 to avoid re-invoking component mounted hook
                for (let i = 1; i < insert.fns.length; i++) {
                  insert.fns[i]()
                }
              }
            } else {
              registerRef(ancestor)
            }
            ancestor = ancestor.parent
          }
        }

        // destroy old node
        if (isDef(parentElm)) {
          removeVnodes([oldVnode], 0, 0)
        } else if (isDef(oldVnode.tag)) {
          invokeDestroyHook(oldVnode)
        }
      }
    }

    invokeInsertHook(vnode, insertedVnodeQueue, isInitialPatch)
    return vnode.elm
  }
}
```

```javascript
export function lifecycleMixin (Vue: Class<Component>) {
  Vue.prototype._update = function (vnode: VNode, hydrating?: boolean) {
    const vm: Component = this
    const prevEl = vm.$el
    const prevVnode = vm._vnode
    const restoreActiveInstance = setActiveInstance(vm)
    vm._vnode = vnode
    // Vue.prototype.__patch__ is injected in entry points
    // based on the rendering backend used.
    if (!prevVnode) {
      // initial render
      vm.$el = vm.__patch__(vm.$el, vnode, hydrating, false /* removeOnly */)
    } else {
      // updates
      vm.$el = vm.__patch__(prevVnode, vnode)
    }
    restoreActiveInstance()
    // update __vue__ reference
    if (prevEl) {
      prevEl.__vue__ = null
    }
    if (vm.$el) {
      vm.$el.__vue__ = vm
    }
    // if parent is an HOC, update its $el as well
    if (vm.$vnode && vm.$parent && vm.$vnode === vm.$parent._vnode) {
      vm.$parent.$el = vm.$el
    }
    // updated hook is called by the scheduler to ensure that children are
    // updated in a parent's updated hook.
  }
}
```