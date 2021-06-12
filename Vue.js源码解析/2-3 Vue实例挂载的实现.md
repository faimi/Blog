
以Runtime + Compiler版的vue.js为例，所以入口文件为`src\platforms\web\entry-runtime-with-compiler.js`

**src\platforms\web\entry-runtime-with-compiler.js文件解析**

（1）先获得原型上的`$mount`方法，用变量`mount`进行缓存，然后再重新定义`$mount`，`Vue.prototype.$mount`方法是在`./runtime/index`文件下定义的，`src\core\instance`下的`vm.$mount(vm.$options.el)`中的`$mount`调用的是`src\platforms\web\entry-runtime-with-compiler.js`的`$mount`函数

问：为什么要重新定义一遍？
答：因为Runtime版本不需要Runtime + Compiler版本的逻辑

（2）先对传入的`el`参数进行处理，它可以是给字符串也可以是`element`，然后调用了`query`方法，看 **./util/index.js文件解析** ，`el = el && query(el)`的`el`就被转换成一个DOM对象。然后对`el`进行判断，如果`el`是`body`或`documentElement`，会进行报错，意思是vue是不能直接挂载到`html`或`body`上。

拿到`$options`，判断有没有定义`render`方法，然后判断有没有写`template`，如果`template`是字符串类型则对`template`进行处理，如果`template`是一个对象的话，就拿`template`的`innerHTML`，如果`template`不是以上两种类型就会有提示，没有`template`就会拿到`el`并调用`getOuterHTML`方法。


```javascript
import Vue from './runtime/index'
import { query } from './util/index'

const mount = Vue.prototype.$mount
Vue.prototype.$mount = function (
  el?: string | Element,
  hydrating?: boolean
): Component {
  el = el && query(el)

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

**./util/index.js文件解析**

如果`el`是字符串，则调用原生API`document.querySelector`方法，然后如果找不到`el`会报错并返回空的`div`，找到的话直接返回`selected`；如果`el`不是字符串，则直接返回`el`

```javascript
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
