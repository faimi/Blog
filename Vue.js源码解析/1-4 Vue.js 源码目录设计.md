关键词: src 目录

# Vue.js 源码目录设计

## vue源码地址

[vue源码地址](https://github.com/vuejs/vue)

## Vue源码目录结构

```javascript
src                 # 源码全在这下面
├── compiler        # 编译相关的代码全在compiler目录
├── core            # 核心代码（灵魂部分）
   ├──components         # 内置组件，例如 keep-alive
   ├──global-api         # 全局API，例如 extend、mixin
   ├──instance           # 例如渲染的辅助函数、事件、初始化、生命周期
   ├──observer           # 与响应式数据相关
   ├──util               # 工具方法
   ├──vdom               # vue2.0核心-->Virtual DOM
├── platforms       # 不同平台的支持，可以跑在web，也可以配合 weex 跑 native
   ├──web                # 浏览器的程序，放于平台相关的代码
   ├──wexx               # 类似于 React Native 这种跨端的应用
├── server          # Node.js 服务端渲染
├── sfc             # 解析器，.vue 文件编译出JavaScript对象
├── shared          # 是可共享的辅助的方法
```

### compiler

Vue 2.0 有一个很大的改进就是有了 Virtual Dom ,它的生成实际上是执行 render function ,但是我们用的时候很少写 render function ,一般都是 template 。 temple --> render function 其实它的相关逻辑是在 compiler 目录。
