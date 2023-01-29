1. `onClick`方法本来可以用，但是现在要在`reqList`后加个`()`才能用

```javascript
<Button type="primary" onClick={() => reqList()}> 查询 </Button>
```

2. 异步函数如何在`modal`打开的时候运行？