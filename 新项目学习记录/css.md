1. 显示一行，超出显示点点点
```css
overflow:hidden; /*超出的部分隐藏起来。*/ 
white-space:nowrap;/*一定要设置width，否则即使超出也不显示...*/
text-overflow:ellipsis;/* 支持 IE */
```
显示两行，超出显示点点点
```css
overflow: hidden;
text-overflow: ellipsis;
display: -webkit-box;
-webkit-line-clamp: 2;
word-break: break-all;  // 注意这个文字多行很重要
-webkit-box-orient: vertical;
```
2.`data-title`的宽度随着`div`的宽度变化而变化