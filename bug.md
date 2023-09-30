### triangle 遍历顶点，i，j弄反(x, y轴反了)， 导致渲染失败
### light_dir normalized后未更新
### intensity可能在[0,1]范围之外，需要特判
### projection投影矩阵不正确导致向量第四个值极小，归一化之后出错

### glfw 设置glbegin的时候GL_POINTS写成了GL_POINT
## view矩阵应为modelView