# Vue.js 源码构建

## Vue 用 Rollup 做构建

[Rollup](https://github.com/rollup/rollup) 和 webpack 一样都是构建工具，webpack 更强大，rollup 更适合 JavaScript 库的编译且更轻量且代码更友好，所以 Vue.js 选了 rollup 做构建，它的构建相关配置都在 scripts 目录下。

## 构建代码

vue 发布在 npm 上，每个 npm 包（相当于一个项目）都需要一个 package.json 文件来对他做描述。

```json
{
  "name": "vue",
  "version": "2.6.13",
  "main": "dist/vue.runtime.common.js",
  "module": "dist/vue.runtime.esm.js",
  "scripts": {
    // 构建相关的任务就是这三个， "build" 是build web平台的 Vue.js，"build ssr" 是输出跟 server renderer 相关，"build weex" 是跟 weex 相关
    "build": "node scripts/build.js",
    "build:ssr": "npm run build -- web-runtime-cjs,web-server-renderer",
    "build:weex": "npm run build -- weex",
  }
}
```

| 字段    | 含义                                                                                                                                            |
| ------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| name    | 名字，是唯一的                                                                                                                                            |
| version | 版本                                                                                                                                            |
| main    | npm 包入口， `import "vue"` 时会通过 main 查找入口                                                                                                                                            |
| module  | module 和 main 非常类似，在 webpack2 以上是把 module 做默认入口，也可以说 Vue.js 的默认入口是 esm.js 文件                                                                                        |
| scripts | npm 提供了 npm scripts ， scripts定义了很多脚本，每个脚本都是一个任务，通过 npm run 值（dev，bulid）可以进行各种不同的脚本执行不同的任务。|

**1、build.js文件解析**

（1）定义依赖的模块

```javascript
const fs = require('fs')
const path = require('path')
const zlib = require('zlib')
const rollup = require('rollup')
const terser = require('terser')

if (!fs.existsSync('dist')) {
  fs.mkdirSync('dist')
}
```

（2）从config.js文件中拿到构建所需要的所有的配置，具体看下面**2、config.js文件解析**

```javascript
let builds = require('./config').getAllBuilds()
```

（3）对配置进行过滤，过滤掉我们不需要的，就剩下我们需要编译的

```javascript
// filter builds via command line arg
if (process.argv[2]) {
  const filters = process.argv[2].split(',')
  builds = builds.filter(b => {
    return filters.some(f => b.output.file.indexOf(f) > -1 || b._name.indexOf(f) > -1)
  })
} else {
  // filter out weex builds by default
  builds = builds.filter(b => {
    return b.output.file.indexOf('weex') === -1
  })
}
```

（4）调用`build`函数做真正的构建过程

```javascript
build(builds)

function build (builds) {
  let built = 0
  const total = builds.length
  const next = () => {
    buildEntry(builds[built]).then(() => {
      built++
      if (built < total) {
        next()
      }
    }).catch(logError)
  }
  next()
}
```

**2、config.js文件解析**

那么上述（2）配置是如何在config.js文件中拿到的的呢？

因为在config.js文件底部暴露了一个方法```exports.getAllBuilds```，这是一个函数，通过```Object.keys(builds)```拿到一个```key```的数组，通过```map```方法调用```genConfig```函数

```javascript
exports.getAllBuilds = () => Object.keys(builds).map(genConfig)
```

（1）builds

`builds`是一个对象，每个`key`对应一个对象，其实就是不同版本vue.js编译的配置

`entry`：入口，通过`resolve`函数传字符串`web/entry-runtime.js`
`dest`：目标
`format`：文件格式，通过不同的```format```可以构建不同的版本
`banner`：变量，定义注释

所以最后以```entry-runtime.js```为编译入口，最后生成```vue.runtime.common.dev.js```文件

```javascript
const builds = {
  'web-runtime-cjs-dev': {
    entry: resolve('web/entry-runtime.js'),
    dest: resolve('dist/vue.runtime.common.dev.js'),
    format: 'cjs',
    env: 'development',
    banner
  }
}
```


（2）resolve

入口：```resolve```函数接收一个参数，在这```base```取值```web```，```p.slice(base.length + 1)```返回```entry-runtime.js```

目标：```base```取值```dist```，在alias.js里没有```dist```，于是直接返回当前目录的大目录，找到 vue.runtime.common.dev.js文件

aliases具体看**3、alias.js文件解析**

```javascript
const aliases = require('./alias')

const resolve = p => {
  const base = p.split('/')[0]
  if (aliases[base]) {
    return path.resolve(aliases[base], p.slice(base.length + 1))
  } else {
    return path.resolve(__dirname, '../', p)
  }
}
```

（3）genConfig

```opts```：```genConfig```获取```key```，```opts```是```builds```中某个对象
```config```：构造一个新的```config```对象，这个对象的数据结构才是真正的```rollup```对应的配置结构，```entry```只是我们自己定义的```entry```，所以要进行转换成```input```

```javascript
function genConfig (name) {
  const opts = builds[name]
  const config = {
    input: opts.entry,
    external: opts.external,
    plugins: [
      flow(),
      alias(Object.assign({}, aliases, opts.alias))
    ].concat(opts.plugins || []),
    output: {
      file: opts.dest,
      format: opts.format,
      banner: opts.banner,
      name: opts.moduleName || 'Vue'
    },
    onwarn: (msg, warn) => {
      if (!/Circular/.test(msg)) {
        warn(msg)
      }
    }
  }
}
```

最终生成的数组返回到build.js文件下，也就是上述（2）

**3、alias.js文件解析**

alias.js文件最后导出一个对象，对象里有很多```key```，通过```resolve```函数返回字符串，字符串是一个目录，```path.resolve```是node.js提供的一个路径解析的方法，```__dirname```是指当前的目录，并往上去找一级，找到大目录，再传参数```p```

因此alias.js文件是提供了一个到最终真实文件地址的一个映射关系

```javascript
const resolve = p => path.resolve(__dirname, '../', p)

module.exports = {
  vue: resolve('src/platforms/web/entry-runtime-with-compiler'),
  compiler: resolve('src/compiler'),
  core: resolve('src/core'),
  shared: resolve('src/shared'),
  web: resolve('src/platforms/web'),
  weex: resolve('src/platforms/weex'),
  server: resolve('src/server'),
  sfc: resolve('src/sfc')
}
```
