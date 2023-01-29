1. `<Button></Button>`的`onClick`写法：
（1）直接`<Button onClick={方法名}></Button>`
（2）传`e`的话`<Button onClick={(e) => 方法名(e)}>修改</Button>`，打印出来是点击事件：`SyntheticBaseEvent {_reactName: 'onClick', _targetInst: null, type: 'click', nativeEvent: PointerEvent, target: span, …}`。
（3）好像不能传两三个参数？