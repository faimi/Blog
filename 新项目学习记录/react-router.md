1.`window.open()`如果想要打开一个新页面，第二个参数要么不填要么`_blank`，不能传其他参数，例如动态的地址
2.想要在react-router里面实现动态路由，在router.tsx中，需要
```javascript
{
    path: '/item-stl',
    title: '3D文件',
    showHeader: false,
    showMenu: false,
    activePath: '/item',
    component: lazy(() => import('../contaicommodityManagement/    CommodityDetail/modules/ItemStl'))
}
```
```javascript
window.open(`${ROUTER_PREFIX}/item/item-stl?url=${name}`, '_blank');
```