# Face Morphing 面部合成

## 导语

### 效果

下面是一段面部合成的过程，演示了从一张脸过渡到另一张脸的过程。

![](https://ws1.sinaimg.cn/large/6177e8b1gy1fevonc859gg208w05017k.gif)

其实实现的原理很简单，就是对两张图片每个像素点按一定的比例进行混合。通过以下的公式来对每个像素点进行融合。

$$ M(x,y) = (1-\alpha)I(x,y) + \alpha J(x,y)$$

其中$$\alpha$$代表了图片J所占比重。但是，如果对两张图片进行融合的话会出现一个问题，就是五官的位置并没有对齐，会出现奇怪的效果，如下图参考1中所展示的图片的问题。会发现效果并不是很好，只是两张图片单纯的叠加。

![](https://ws1.sinaimg.cn/large/6177e8b1gy1fexljkov27j20qq0buk7w.jpg)

这次实验的主要内容，就是介绍如何进行比较和谐的一种融合效果，已经将多张不同比重融合的图片所组成的一个人脸变换过程。

## 实现

### 实验环境

Windows 10、Visual Studio 2015、CImg图像处理库

### 实验数据

![](https://ws1.sinaimg.cn/large/6177e8b1gy1fexn2sh0hdj20ys0mu7wh.jpg)

rate = 0.6时的效果：

<img src="https://ws1.sinaimg.cn/large/6177e8b1gy1fez3cx8b0fj20br0g3jvc.jpg" width="300"/>

### 主要步骤

1. 构建人脸特征点
2. 进行Delaunay三角剖分
3. 将每个Delaunay三角形对映射到同一个三角形区域
4. 对每个映射三角形区域进行morphing

### 详细步骤

##### 1. 构建人脸特征点

![](https://ws1.sinaimg.cn/large/6177e8b1gy1fexnir9ud1j20w80lc1kx.jpg)

这个实验的重点不是实现人脸特征点的识别，所以这里就采用人工进行采集特征点。也可以采用Dlib库来进行特征点提取，但是这个库需要用到opencv，这里使用的库是CImg，所以没有采用。

```c++
struct point {
    int x;
    int y;
    point(int _x, int _y) : x(_x), y(_y) {}
};
/*
* get dection points to vector<point> manually
* this function support to reuse the records we have record in fliename .txt
* if you want to detect the points by yourself just remove the txt file
* record method from outer loop in inner loop
* in this example 24 points each picture
*/
CImg<float> getDetectionPoints(vector<point*>& points, const CImg<float>& src, const string filename) {
    // 1.jpg to 1.txt which record the points
    string points_file(filename);
    points_file.replace(points_file.end() - 3, points_file.end(), "txt");
    CImg<float> dect_img = src;
    int color[] = { 255, 0, 0 };

    // try to read detection points from file
    ifstream input(points_file);
    ofstream output;
    if (input.fail()) { // if no exist points
        output.open(points_file);
        CImgDisplay disp(dect_img, filename.c_str());

        while (!disp.is_closed()) {
            disp.wait();
            if (disp.button() & 1 && disp.mouse_y() >= 0) {
                point* p = new point(disp.mouse_x(), disp.mouse_y());
                points.push_back(p);
                // draw circle
                dect_img.draw_circle(p->x, p->y, dect_img._width / 100, color);
                dect_img.display(disp);
                // write file
                output << p->x << "," << p->y << endl;
            }
        }
        output.close();
    }
    else {
        string line;
        while (getline(input, line)) {
            // point format : x,y
            int pos = line.find(',');
            if (pos != string::npos) {
                // skip ","
                string x_str = line.substr(0, pos);
                string y_str = line.substr(pos + 1, string::npos);
                point* p = new point(std::stoi(x_str), std::stoi(y_str));
                points.push_back(p);
                // draw circle
                dect_img.draw_circle(p->x, p->y, dect_img._width / 100, color);
            }
        }
        //dect_img.display();
    }
    input.close();
    return dect_img;
}
```

这里首先读取与文件 名相同的`txt`文件来读取上一次侦测的记录，若之前对这张图片没有进行过特征点的手动记录，则跳出窗口来对特征点进行手动的侦测，然后存入`txt`文件中以便下次再读取。然后把读取到的侦测点存储在`vector`容器中。

除了手动检测的这几个点，还给每张图片加入了8个点，分别是4个角点和每条片的中点。

```c++
    // add four corners and four middle points
    points_A.push_back(new point(0, 0));
    points_A.push_back(new point(0, img_A._height));
    points_A.push_back(new point(img_A._width, 0));
    points_A.push_back(new point(img_A._width, img_A._height));
    points_A.push_back(new point(0, img_A._height/2));
    points_A.push_back(new point(img_A._width/2, 0));
    points_A.push_back(new point(img_A._width, img_A._height/2));
    points_A.push_back(new point(img_A._width/2, img_A._height));

    points_B.push_back(new point(0, 0));
    points_B.push_back(new point(0, img_B._height));
    points_B.push_back(new point(img_B._width, 0));
    points_B.push_back(new point(img_B._width, img_B._height));
    points_B.push_back(new point(0, img_B._height/2));
    points_B.push_back(new point(img_B._width/2, 0));
    points_B.push_back(new point(img_B._width, img_B._height / 2));
    points_B.push_back(new point(img_B._width/2, img_B._height));
```



##### 2. 进行Delaunay三角剖分

![](https://ws1.sinaimg.cn/large/6177e8b1gy1fez20cah7hj20ty0je1kx.jpg)

Delaunay 三角形剖分是一种最常用的三角剖分算法，具体可以参考5和百度百科。其中有一个定义

> 假设V是二维实数域上的有限点集，边e是由点集中的点作为端点构成的封闭线段, E为e的集合。那么该点集V的一个三角剖分T=(V,E)是一个平面图G
>
> 假设T为V的任一三角剖分，则T是V的一个Delaunay三角剖分，当前仅当T中的每个三角形的外接圆的内部不包含V中任何的点。

常用的Delaunay三角形的算法，有翻边算法、逐点插入算法、分割合并算法、Bowyer-Watson算法等。

这里考虑到特征检测的点比较少，就直接采用上述关于外接圆的这个定义进行暴力验证，时间复杂度达`O(n^4)`

```c++
struct triangle {
    point a;
    point b;
    point c;
    int index[3];
    triangle(point _a, point _b, point _c, int a_index, int b_index, int c_index)
        : a(_a), b(_b), c(_c) {
        index[0] = a_index;
        index[1] = b_index;
        index[2] = c_index;
    }
    triangle(point _a, point _b, point _c) : a(_a), b(_b), c(_c) {}

    bool isInTriangle(const point& p);
};
// get Delaunay triangles by traverse
void getDelaunayTriangles(vector<triangle*>& triangles, const vector<point*>& points) {
    int size = points.size();
    if (size < 3) return;

    for (int i = 0; i < size - 2; i++) {
        for (int j = i + 1; j < size - 1; j++) {
            for (int k = j + 1; k < size; k++) {
                point* A = points[i];
                point* B = points[j];
                point* C = points[k];
                triangle* tri = new triangle(*A, *B, *C, i, j, k);

                bool no_exist = true;
                for (int m = 0; m < size; m++)
                {
                    point* P = points[m];
                    if (m == i || m == j || m == k) continue;
                    double k1 = (double)(A->y - B->y) / (A->x - B->x);
                    double k2 = (double)(A->y - C->y) / (A->x - C->x);
                    if (isPointInCircle(P, tri) || abs(k1-k2) < 0.1) // abs to avoid to closer points
                    {
                        no_exist = false;
                        break;
                    }
                }
                if (no_exist)
                {
                    triangles.push_back(tri);
                }
            }
        }
    }

}
```

其实也就是验证每个三角形，看其外接圆内是否包含其他的点，若包含则不要这个三角形，若不包含则要。这里每个点的也同时记录下了其序号，只要对其中一张图片运算Delaunay三角剖分，再另一张图片中只需要根据所剖分出来的三角形的序号进行三角形的构建即可，因为两张图片的点位置是相对应的。

```c++
// get couterpart delaunay triangles of B from A
for (int i = 0; i < triangles_A.size(); i++) {
    triangles_B.push_back(new triangle(*points_B.at(triangles_A[i]->index[0]),
                            *points_B.at(triangles_A[i]->index[1]),
                            *points_B.at(triangles_A[i]->index[2]),
                            triangles_A[i]->index[0],
                            triangles_A[i]->index[1],
                            triangles_A[i]->index[2]));
}
```



其中还涉及到了如何验证一个三角形的外接圆是否包含某个点。这里使用了三角形外心的公式来进行计算，直接根据三角形的三个点推出外心坐标，再计算该点与外心的距离是否小于等于半径，如果满足则在外接圆内或上有点。具体公式可以看参考3维基百科中所给的公式。

```c++
// get distance between point A and B
double getDistance(point A, point B) {
    double dis = (A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y);
    return sqrt(dis);
}
// to judge is point p in the circumcircle of tri
// formular find in wikipedia Circumscribed circle
bool isPointInCircle(point* P, triangle* tri) {
    point origin(0, 0);
    double a_x = tri->a.x;
    double b_x = tri->b.x;
    double c_x = tri->c.x;
    double a_y = tri->a.y;
    double b_y = tri->b.y;
    double c_y = tri->c.y;
    double D = 2 * (a_x*(b_y-c_y) + b_x*(c_y-a_y) + c_x*(a_y-b_y));
    double x = ((a_x*a_x + a_y*a_y)*(b_y - c_y) + (b_x*b_x + b_y*b_y)*(c_y - a_y) + (c_x*c_x + c_y*c_y)*(a_y - b_y)) / D;
    double y = ((a_x*a_x + a_y*a_y)*(c_x - b_x) + (b_x*b_x + b_y*b_y)*(a_x - c_x) + (c_x*c_x + c_y*c_y)*(b_x - a_x)) / D;
    point center((int)x, (int)y);
    double radius = getDistance(center, tri->a);

    return (getDistance(*P, center) <= radius);
}
```



##### 3.将每个Delaunay三角形对映射到同一个三角形区域

上一步结束之后，我们便得到了一些相对应的Delaunay三角形对。接下来要做的就是在每个三角形对之间，求一个的过渡的三角形，然后将三角形对在这个过渡三角形上进行融合。这里过渡三角形的大小是根据两张图片的比例来进行加权的。这样子有一个缺点，就是如果两张图片大小不同，其不同比例的融合的图片大小也会不一样。

```c++
// transition between source A and B
triangle* getTransitionTriangle(const triangle* A, const triangle* B, double rate) {
    int ax = (int)(rate*(A->a.x) + (1 - rate)*(B->a.x));
    int ay = (int)(rate*(A->a.y) + (1 - rate)*(B->a.y));
    int bx = (int)(rate*(A->b.x) + (1 - rate)*(B->b.x));
    int by = (int)(rate*(A->b.y) + (1 - rate)*(B->b.y));
    int cx = (int)(rate*(A->c.x) + (1 - rate)*(B->c.x));
    int cy = (int)(rate*(A->c.y) + (1 - rate)*(B->c.y));
    return new triangle(point(ax, ay), point(bx, by), point(cx, cy));
}
```

函数`getTransTriangle2Triangle`是根据源三角形到目标三角形来求其变换矩阵的。根据公式$HA=D$，两边右乘以A的逆矩阵即可以得到变换矩阵H。

```c++
void getTransTriangle2Triangle(double(*H)[3], const triangle* src, const triangle* dst) {

    // transform src to dst
    int u0 = src->a.x;
    int v0 = src->a.y;
    int u1 = src->b.x;
    int v1 = src->b.y;
    int u2 = src->c.x;
    int v2 = src->c.y;

    int x0 = dst->a.x;
    int y0 = dst->a.y;
    int x1 = dst->b.x;
    int y1 = dst->b.y;
    int x2 = dst->c.x;
    int y2 = dst->c.y;

    // |A|
    int detA;
    detA = u0*v1 + u1*v2 + u2*v0 - u2*v1 - u0*v2 - u1*v0;
    // inv(A) * detA
    int A11, A12, A13, A21, A22, A23, A31, A32, A33;
    A11 = v1 - v2;
    A21 = -(v0 - v2);
    A31 = v0 - v1;
    A12 = -(u1 - u2);
    A22 = u0 - u2;
    A32 = -(u0 - u1);
    A13 = u1*v2 - u2*v1;
    A23 = -(u0*v2 - u2*v0);
    A33 = u0*v1 - u1*v0;

    H[0][0] = (double)(x0*A11 + x1*A21 + x2*A31) / detA;
    H[1][0] = (double)(y0*A11 + y1*A21 + y2*A31) / detA;
    H[2][0] = (double)(A11 + A21 + A31) / detA;
    H[0][1] = (double)(x0*A12 + x1*A22 + x2*A32) / detA;
    H[1][1] = (double)(y0*A12 + y1*A22 + y2*A32) / detA;
    H[2][1] = (double)(A12 + A22 + A32) / detA;
    H[0][2] = (double)(x0*A13 + x1*A23 + x2*A33) / detA;
    H[1][2] = (double)(y0*A13 + y1*A23 + y2*A33) / detA;
    H[2][2] = (double)(A13 + A23 + A33) / detA;
}
```

需要将求过渡三角形变换到两张图对应的Delaunay的变换矩阵两个，然后将过度三角形的坐标转换成在两张图片中对应的坐标。这样子做法的一个好处就是过渡三角形上不会有没有映射到的点出现。



##### 4.对每个映射三角形区域进行morphing

对每个三角形对求变换到两个Delaunay三角形的变换矩阵，然后将对每个点进行转换和融合(morph), 这里涉及到了一个遍历三角形的问题，原先是想求出三角形的最小外接矩形这样子就可以直接遍历这个矩形中的点，再判断这个点是否在三角形内了，但是求这个矩形是遇到了一些困难，于是就改成了直接对遍历全部的点，判断是否在这个三角形内。当图片大的时候，这样子其实效率是很低的。

这里涉及到了判断一个点是否在三角形内的方法。这里采用的是向量叉积符号的方法，具体可以看参考4。如果三角形的逆时针边的向量的与点P的叉积都是正的，则可以判断P在三角形内，但是由于无法判断我们三角形拿出来的边是顺时针还是逆时针，我就改成了判断三个叉积的符号是否相同，如果相同则可以判断点在三角形内。

![](https://ws1.sinaimg.cn/large/6177e8b1gy1fez32vqseuj20vg0atq40.jpg)

```c++
int cross3(const point &a, const point &b, const point &p) {
    return (b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x);
}
bool triangle::isInTriangle(const point& p) {

    if (cross3(a, b, p) >= 0 && cross3(b, c, p) >= 0 && cross3(c, a, p) >= 0)
        return true;
    else if (cross3(a, b, p) <= 0 && cross3(b, c, p) <= 0 && cross3(c, a, p) <= 0)
        return true;
    else
        return false;
}
```

在求完过渡三角形的每个点在两张图片上对应的点之后(使用变换矩阵求出坐标之后再使用双线性内插)，就可以根据公式 $$M(x,y) = (1-\alpha)I(x,y) + \alpha J(x,y)$$来进行morphing了。

```c++
// morph Triangles
CImg<float> morphTriangle(double rate, const vector<triangle*>& triangles_A, const vector<triangle*>& triangles_B,
    const CImg<float>& img_A, const CImg<float>& img_B) {
    int width = (rate * img_A._width + (1 - rate) * img_B._width);
    int height = (rate * img_A._height + (1 - rate) * img_B._height);
    CImg<float> result(width, height, 1, 3);

    for (int i = 0; i < triangles_A.size(); i++) {
        triangle* trans_tri = getTransitionTriangle(triangles_A[i], triangles_B[i], rate);
        double H1[3][3], H2[3][3];
        getTransTriangle2Triangle(H1, trans_tri, triangles_A[i]);
        getTransTriangle2Triangle(H2, trans_tri, triangles_B[i]);
        cimg_forXY(result, x, y) {
            if (trans_tri->isInTriangle(point(x, y))) {

                float tx_a = x*H1[0][0] + y*H1[0][1] + H1[0][2];
                float ty_a = x*H1[1][0] + y*H1[1][1] + H1[1][2];
                float pixel_a[3] = { 0 };
                cimg_forC(img_A, c) {
                    pixel_a[c] = img_A.linear_atXY(tx_a, ty_a, 0, c);
                }

                float tx_b = x*H2[0][0] + y*H2[0][1] + H2[0][2];
                float ty_b = x*H2[1][0] + y*H2[1][1] + H2[1][2];
                float pixel_b[3] = { 0 };
                cimg_forC(img_B, c) {
                    pixel_b[c] = img_B.linear_atXY(tx_b, ty_b, 0, c);
                }

                // morph
                cimg_forC(result, c) {
                    result(x, y, 0, c) = rate*pixel_a[c] + (1 - rate)*pixel_b[c];
                }
            }

        }
    }

    return result;
}
```



main 函数

```c++
int main() {

    // --- step 1 : read img ---

    string filename_A = "img/1.jpg";
    string filename_B = "img/2.jpg";
    CImg<float> img_A(filename_A.c_str());
    CImg<float> img_B(filename_B.c_str());
    
    // --- step 2 : get dection points ---
    vector<point*> points_A, points_B;

    // add four corners and four middle points
    points_A.push_back(new point(0, 0));
    points_A.push_back(new point(0, img_A._height));
    points_A.push_back(new point(img_A._width, 0));
    points_A.push_back(new point(img_A._width, img_A._height));
    points_A.push_back(new point(0, img_A._height/2));
    points_A.push_back(new point(img_A._width/2, 0));
    points_A.push_back(new point(img_A._width, img_A._height/2));
    points_A.push_back(new point(img_A._width/2, img_A._height));

    points_B.push_back(new point(0, 0));
    points_B.push_back(new point(0, img_B._height));
    points_B.push_back(new point(img_B._width, 0));
    points_B.push_back(new point(img_B._width, img_B._height));
    points_B.push_back(new point(0, img_B._height/2));
    points_B.push_back(new point(img_B._width/2, 0));
    points_B.push_back(new point(img_B._width, img_B._height / 2));
    points_B.push_back(new point(img_B._width/2, img_B._height));

    getDetectionPoints(points_A, img_A, filename_A);
    getDetectionPoints(points_B, img_B, filename_B);

    if (points_A.size() != points_B.size()) {
        throw ("the decetion points size are no the same");
    }
    // --- step 3 : get Delaunay triangles --- 
    vector<triangle*> triangles_A, triangles_B;
    getDelaunayTriangles(triangles_A, points_A);

    // get couterpart delaunay triangles of B from A
    for (int i = 0; i < triangles_A.size(); i++) {
        triangles_B.push_back(new triangle(*points_B.at(triangles_A[i]->index[0]),
                                *points_B.at(triangles_A[i]->index[1]),
                                *points_B.at(triangles_A[i]->index[2]),
                                triangles_A[i]->index[0],
                                triangles_A[i]->index[1],
                                triangles_A[i]->index[2]));
    }
    drawTriangles(triangles_A, img_A);
    drawTriangles(triangles_B, img_B);

    // --- step 4: morph ---
    double rate = 0;
    for (int i = 0; i <= 10; i++) {
        CImg<float> frame = morphTriangle(rate, triangles_A, triangles_B, img_A, img_B);
        rate += 0.1;
        string filename = "img/frame/.jpg";
        if (i < 10) {
            filename.insert(10, "0");
            filename.insert(11, std::to_string(i));
        }
        else {
            filename.insert(10, std::to_string(i));
        }

        frame.save(filename.c_str());
        cout << "saving frame " << i << endl;
    }
    return 0;
}
```



### 总结

rate = 0.6时候的效果

<img src="https://ws1.sinaimg.cn/large/6177e8b1gy1fez3cx8b0fj20br0g3jvc.jpg" width="300"/>

这次做的模型还是一个比较基础的模型，在侦测点不多的情况下速度平平，如果要做成工业级的产品，还有诸多需要改进的地方。比如Delaunay三角剖分的算法、遍历三角形的算法、人脸特征点的快速侦测。其实做完之后发现原理很简单，无非就是一个变换公式。主要做的功夫都是要使得”对应“的点能够对应着进行转换，才能达到一个比较好的效果。

## 参考

1. [手把手：使用OpenCV进行面部合成— C++ / Python](http://mp.weixin.qq.com/s?__biz=MjM5MTQzNzU2NA==&mid=2651641340&idx=1&sn=29e39cb6113120fe73b3c397f1c9d555)
2. [基于opencv+Dlib的面部合成（Face Morph）](http://blog.csdn.net/wangxing233/article/details/51549880)
3. [Wikipedia : Circumscribed circle](https://en.wikipedia.org/wiki/Circumscribed_circle)
4. [向量叉积和应用：判断点是否在三角形内部](http://www.yalewoo.com/in_triangle_test.html)
5. [Delaunay三角剖分算法](http://www.cnblogs.com/zhiyishou/p/4430017.html)