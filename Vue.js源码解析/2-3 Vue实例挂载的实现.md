问题：`src/core/instance/init.js`的`$mount`是从哪来的？

关键词：`$mount`、`render`、`template`

逻辑：`mountComponent`会完成整个渲染工作，最核心的2个方法：`vm._render`和`vm._update`。

# Vue实例挂载的实现

也就是执行`vm.$mount`方法做了那些事情？

以 Runtime + Compiler 版的 vue.js 为例，所以入口文件为`src/platforms/web/entry-runtime-with-compiler.js`

## 重写`Vue.prototype.$mount`

### 没有`render`

（1）先获得 Vue 原型上的`$mount`方法，用变量`mount`进行缓存，然后再重新定义`$mount`。 **Vue 原型上的`$mount`方法是在`src/platforms/web/runtime/index.js`文件下定义的。**

问：为什么在`src/platforms/web/runtime/index.js`文件下定义的`$mount`方法要在`src/platforms/web/entry-runtime-with-compiler.js`文件重新定义一遍？
答：因为 Runtime Only 版本不需要 Runtime + Compiler 版本的逻辑。**`src/core/instance/init.js`下的`vm.$mount(vm.$options.el)`中的`$mount`调用的是`src/platforms/web/entry-runtime-with-compiler.js`的`$mount`函数。**

（2）先对传入的`el`参数进行处理，它可以是给字符串也可以是`Element`，然后调用了`query`方法，`query`方法在`src/platforms/web/util/index.js`文件中有定义。

在`src/platforms/web/util/index.js`文件中，如果`el`是字符串，则调用原生 API ——`document.querySelector()`方法，如果找不到`el`会报错并返回空的`div`，找到的话直接返回`document.querySelector(el)`；如果`el`不是字符串，已经是个 DOM 对象则直接返回`el`。

- `document.querySelector()`方法：`console.log(document.querySelector("#app"))`--->`<div id="app">Hello Vue!</div>`

（3）`el = el && query(el)`的`el`就被转换成一个 DOM 对象。然后对`el`进行判断，如果`el`是`body`或`documentElement`（文档标签），会进行报错，意思是 Vue 是不能直接挂载到`html`或`body`上，因为挂载会把整个body覆盖，整个 HTML 文档会发生错误。所以我们通常都是`<div id="app"></div>`方式去实现而不是直接挂载在 body 下。

```javascript
var a = '#app',b = document.querySelector("#app");
var c = a && b,d = b && a;
console.log(c); // <div id="app">Hello Vue!</div>
console.log(d); // #app
```

```javascript
// src/platforms/web/util/index.js
export function query (el: string | Element): Element {
  if (typeof el === 'string') {
    const selected = document.querySelector(el)
    if (!selected) {
      process.env.NODE_ENV !== 'production' && warn(
        'Cannot find element: ' + el
      )
      return document.createElement('div')
    }
    return selected
  } else {
    return el
  }
}
```

（4）拿到`$options`，判断有没有定义`render`方法，没有定义`render`方法则拿到`template`，如果`template`存在，如果`template`是字符串类型则对`template`进行处理，如果`template`是一个 DOM 对象的话，就拿`template`的`innerHTML`，如果`template`不是以上两种类型就会有警告；没有`template`就会拿到`el`并调用`getOuterHTML`方法。

`getOuterHTML`方法拿到`el`的`outerHTML`,如果有就直接返回，否则在`el.outerHTML`外包一层`div`，再执行`innerHTML`，返回字符串。然后就是编译过程了。此时`template`是`"<div id="app">{{ message }}</div>"`（字符串类型）

```javascript
function getOuterHTML (el: Element): string {
  if (el.outerHTML) {
    return el.outerHTML
  } else {
    const container = document.createElement('div')
    container.appendChild(el.cloneNode(true))
    return container.innerHTML
  }
}
```

接下来的部分与编译相关。

```javascript
// src/platforms/web/entry-runtime-with-compiler.js
import Vue from './runtime/index'
import { query } from './util/index'

const mount = Vue.prototype.$mount
Vue.prototype.$mount = function (
  el?: string | Element,
  hydrating?: boolean
): Component {
  el = el && query(el) //内部判断el是否是字符串，不是的话就直接返回

  /* istanbul ignore if */
  if (el === document.body || el === document.documentElement) {
    process.env.NODE_ENV !== 'production' && warn(
      `Do not mount Vue to <html> or <body> - mount to normal elements instead.`
    )
    return this
  }

  const options = this.$options
  // resolve template/el and convert to render function
  if (!options.render) {
    let template = options.template
    if (template) {
      if (typeof template === 'string') {
        if (template.charAt(0) === '#') {
          template = idToTemplate(template)
          /* istanbul ignore if */
          if (process.env.NODE_ENV !== 'production' && !template) {
            warn(
              `Template element not found or is empty: ${options.template}`,
              this
            )
          }
        }
      } else if (template.nodeType) {
        template = template.innerHTML
      } else {
        if (process.env.NODE_ENV !== 'production') {
          warn('invalid template option:' + template, this)
        }
        return this
      }
    } else if (el) {
      template = getOuterHTML(el)
    }
    if (template) {
      /* istanbul ignore if */
      if (process.env.NODE_ENV !== 'production' && config.performance && mark) {
        mark('compile')
      }

      const { render, staticRenderFns } = compileToFunctions(template, {
        outputSourceRange: process.env.NODE_ENV !== 'production',
        shouldDecodeNewlines,
        shouldDecodeNewlinesForHref,
        delimiters: options.delimiters,
        comments: options.comments
      }, this)
      options.render = render
      options.staticRenderFns = staticRenderFns

      /* istanbul ignore if */
      if (process.env.NODE_ENV !== 'production' && config.performance && mark) {
        mark('compile end')
        measure(`vue ${this._name} compile`, 'compile', 'compile end')
      }
    }
  }
  return mount.call(this, el, hydrating)
}
```

总结：如果没有定义`render`方法，则会把`el`或者`template`字符串转换成`render`方法。这里我们要牢记，在 Vue2.0 版本中，所有`Vue`的组件的渲染最终都需要`render`方法，无论我们是用单文件.vue方式开发组件，还是写了`el`或者`template`属性，最终都会转换成`render`方法。

### 有`render`

（1）如果有`render`函数直接调用`mount`方法，这个`mount`方法是之前缓存的`$mount`，因此又回到了`src/platforms/web/runtime/index.js`文件中，在该文件中调用`mountComponent`方法。`mountComponent`方法来自`src/core/instance/lifecycle.js`文件。

```javascript
return mount.call(this, el, hydrating)
```

（2）在`src/core/instance/lifecycle.js`文件中，将`el`（DOM）缓存到`vm.$el`，然后判断有没有`render`函数，没有`render`函数并且`template`没有准确转换成`render`函数，则创建一个`createEmptyVNode`（空 VNode ），开发环境会报警告。①例如定义了`template`但是第一个字符不是“#”，即在用 runtime-only 版本，又写了`template options render`函数，就会报这个警告。②还有就是写了`template`，但是没用这个编译版本，所以不会生成`render`函数，也会报错。即用了 runtime-only 版本，但是没有直接写`render`函数，`template`不可编译。③还有一种情况是没有`template`也没有`render`函数，会报`else`下的警告。**总的来说就是没正确生成`render`函数。** Vue 只认 render 函数。

然后定义了`updateComponent`函数，函数调用了`vm._update`,`vm._update`的第一个参数是`vm._render()`渲染出来的 VNode ，`hydrating`跟渲染相关，可以认为是`false`。`updateComponent`的执行是调用了`new Watcher`，这是一个渲染`Watcher`。`Watcher`是跟响应式原理强相关的类，实际上是一个观察者模式，有很多自定义`Watcher`，其中一个是渲染`Watcher`。`Watcher`的定义在`src/core/observer/watcher.js`文件里。

（3）`Watcher`实际上也是一个 class ，传入的第一个参数是`vm`，就是当前的 vue 实例。然后是`updateComponent`函数，第三个是`noop`（空 function ），第四个是配置，也是对象。第五个是个 Boolean 值。跟`constructor`的定义一一对应。`isRenderWatcher`是否是渲染 Watcher 的标志位。传 true 时，就在`vm`上增加`_watcher`，然后把所有都`push`到`vm._watchers`里。

如果`expOrFn`是函数，则 getter 复制函数，否则调用`parsePath`把`expOrFn`转化成`getter`。会在`get()`中调用一次`getter`，即`value = this.getter.call(vm, vm)`，就执行到`updateComponent`方法了，就会执行`vm._update()`和`vm._render()`。`vm._render`方法先生成虚拟 Node ，最终调用`vm._update`更新 DOM 。

```javascript
// src/core/observer/watcher.js
export default class Watcher {
  vm: Component;
  expression: string;
  cb: Function;
  id: number;
  deep: boolean;
  user: boolean;
  lazy: boolean;
  sync: boolean;
  dirty: boolean;
  active: boolean;
  deps: Array<Dep>;
  newDeps: Array<Dep>;
  depIds: SimpleSet;
  newDepIds: SimpleSet;
  before: ?Function;
  getter: Function;
  value: any;

  constructor (
    vm: Component,
    expOrFn: string | Function,
    cb: Function,
    options?: ?Object,
    isRenderWatcher?: boolean
  ) {
    this.vm = vm
    if (isRenderWatcher) {
      vm._watcher = this
    }
    vm._watchers.push(this)
    // options
    if (options) {
      this.deep = !!options.deep
      this.user = !!options.user
      this.lazy = !!options.lazy
      this.sync = !!options.sync
      this.before = options.before
    } else {
      this.deep = this.user = this.lazy = this.sync = false
    }
    this.cb = cb
    this.id = ++uid // uid for batching
    this.active = true
    this.dirty = this.lazy // for lazy watchers
    this.deps = []
    this.newDeps = []
    this.depIds = new Set()
    this.newDepIds = new Set()
    this.expression = process.env.NODE_ENV !== 'production'
      ? expOrFn.toString()
      : ''
    // parse expression for getter
    if (typeof expOrFn === 'function') {
      this.getter = expOrFn
    } else {
      this.getter = parsePath(expOrFn)
      if (!this.getter) {
        this.getter = noop
        process.env.NODE_ENV !== 'production' && warn(
          `Failed watching path: "${expOrFn}" ` +
          'Watcher only accepts simple dot-delimited paths. ' +
          'For full control, use a function instead.',
          vm
        )
      }
    }
    this.value = this.lazy
      ? undefined
      : this.get()
  }

  /**
   * Evaluate the getter, and re-collect dependencies.
   */
  get () {
    pushTarget(this)
    let value
    const vm = this.vm
    try {
      value = this.getter.call(vm, vm)
    } catch (e) {
      if (this.user) {
        handleError(e, vm, `getter for watcher "${this.expression}"`)
      } else {
        throw e
      }
    } finally {
      // "touch" every property so they are all tracked as
      // dependencies for deep watching
      if (this.deep) {
        traverse(value)
      }
      popTarget()
      this.cleanupDeps()
    }
    return value
  }

  /**
   * Add a dependency to this directive.
   */
  addDep (dep: Dep) {
    const id = dep.id
    if (!this.newDepIds.has(id)) {
      this.newDepIds.add(id)
      this.newDeps.push(dep)
      if (!this.depIds.has(id)) {
        dep.addSub(this)
      }
    }
  }

  /**
   * Clean up for dependency collection.
   */
  cleanupDeps () {
    let i = this.deps.length
    while (i--) {
      const dep = this.deps[i]
      if (!this.newDepIds.has(dep.id)) {
        dep.removeSub(this)
      }
    }
    let tmp = this.depIds
    this.depIds = this.newDepIds
    this.newDepIds = tmp
    this.newDepIds.clear()
    tmp = this.deps
    this.deps = this.newDeps
    this.newDeps = tmp
    this.newDeps.length = 0
  }

  /**
   * Subscriber interface.
   * Will be called when a dependency changes.
   */
  update () {
    /* istanbul ignore else */
    if (this.lazy) {
      this.dirty = true
    } else if (this.sync) {
      this.run()
    } else {
      queueWatcher(this)
    }
  }

  /**
   * Scheduler job interface.
   * Will be called by the scheduler.
   */
  run () {
    if (this.active) {
      const value = this.get()
      if (
        value !== this.value ||
        // Deep watchers and watchers on Object/Arrays should fire even
        // when the value is the same, because the value may
        // have mutated.
        isObject(value) ||
        this.deep
      ) {
        // set new value
        const oldValue = this.value
        this.value = value
        if (this.user) {
          const info = `callback for watcher "${this.expression}"`
          invokeWithErrorHandling(this.cb, this.vm, [value, oldValue], this.vm, info)
        } else {
          this.cb.call(this.vm, value, oldValue)
        }
      }
    }
  }

  /**
   * Evaluate the value of the watcher.
   * This only gets called for lazy watchers.
   */
  evaluate () {
    this.value = this.get()
    this.dirty = false
  }

  /**
   * Depend on all deps collected by this watcher.
   */
  depend () {
    let i = this.deps.length
    while (i--) {
      this.deps[i].depend()
    }
  }

  /**
   * Remove self from all dependencies' subscriber list.
   */
  teardown () {
    if (this.active) {
      // remove self from vm's watcher list
      // this is a somewhat expensive operation so we skip it
      // if the vm is being destroyed.
      if (!this.vm._isBeingDestroyed) {
        remove(this.vm._watchers, this)
      }
      let i = this.deps.length
      while (i--) {
        this.deps[i].removeSub(this)
      }
      this.active = false
    }
  }
}
```

```javascript
// src/core/instance/lifecycle.js
export function mountComponent (
  vm: Component,
  el: ?Element,
  hydrating?: boolean
): Component {
  vm.$el = el
  if (!vm.$options.render) {
    vm.$options.render = createEmptyVNode
    if (process.env.NODE_ENV !== 'production') {
      /* istanbul ignore if */
      if ((vm.$options.template && vm.$options.template.charAt(0) !== '#') ||
        vm.$options.el || el) {
        warn(
          'You are using the runtime-only build of Vue where the template ' +
          'compiler is not available. Either pre-compile the templates into ' +
          'render functions, or use the compiler-included build.',
          vm
        )
      } else {
        warn(
          'Failed to mount component: template or render function not defined.',
          vm
        )
      }
    }
  }
  callHook(vm, 'beforeMount')

  let updateComponent
  /* istanbul ignore if */
  if (process.env.NODE_ENV !== 'production' && config.performance && mark) {
    updateComponent = () => {
      const name = vm._name
      const id = vm._uid
      const startTag = `vue-perf-start:${id}`
      const endTag = `vue-perf-end:${id}`

      mark(startTag)
      const vnode = vm._render()
      mark(endTag)
      measure(`vue ${name} render`, startTag, endTag)

      mark(startTag)
      vm._update(vnode, hydrating)
      mark(endTag)
      measure(`vue ${name} patch`, startTag, endTag)
    }
  } else {
    updateComponent = () => {
      vm._update(vm._render(), hydrating)
    }
  }

  // we set this to vm._watcher inside the watcher's constructor
  // since the watcher's initial patch may call $forceUpdate (e.g. inside child
  // component's mounted hook), which relies on vm._watcher being already defined
  //渲染watch
  new Watcher(vm, updateComponent, noop, {
    before () {
      if (vm._isMounted && !vm._isDestroyed) {
        callHook(vm, 'beforeUpdate')
      }
    }
  }, true /* isRenderWatcher */)
  hydrating = false

  // manually mounted instance, call mounted on self
  // mounted is called for render-created child components in its inserted hook
  if (vm.$vnode == null) {
    vm._isMounted = true
    callHook(vm, 'mounted')
  }
  return vm
}
```



总而言之，**先对`el`做解析，判断`render function`，没有`render`则转换成`template`，`template`最终会编译成`render`**。也就是说 Vue 只认`render`函数。

主要了解了`$mount`的实现，在`compiler`版本下，是对`el`做处理然后如果没定义`render`函数获取`render`函数，也就是把`template`通过一系列逻辑判断转换成`render`。**`template`有很多种写法，可以写`template`，也可以`template`是一个 dom ，不写的话通过`el`获取`template`，然后把`template`通过编译的手段转化成`render`函数。**也就是说`$mount`在`compiler`版本的前提就是首先拿到`render`函数，然后调用`mountComponent`方法。`mountComponent`方法定义了`updateComponent`函数，这个函数实际上是一个渲染`Watcher`。通过`Watcher`是因为`updateComponent`实际上是执行了一次渲染，这个渲染过程除了首次渲染之后在更新数据还会触发`updateComponent`方法。`Watcher`在这里起到两个作用，一个是初始化的时候会执行回调函数，另一个是当`vm`实例中的监测的数据发生变化的时候执行回调函数。`updateComponent`是监听到执行的一个过程，当数据发生变化，视图修改入口也是`updateComponent`方法。这就是渲染`Watcher`要做的事情。

# 什么是 DOM 对象

在DOM中，每一个元素看成一个节点，而每一个节点就是一个“对象”。也就是我们在操作元素时，把每一个元素节点看成一个对象，然后使用这个对象的属性和方法进行相关操作。一个 div 就相当于一个 DOM 元素

```html
<html>
<head>
<title><title>
<body>
<h1>.....</h1>
<p>.....</p>
</body>
</html>
```
