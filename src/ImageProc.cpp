#include "ImageProc.h"
#include "time.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

#define CAPS 26
#define SPACE 27

CImageProc::CImageProc() {}
CImageProc::~CImageProc() {}

const vector<vector<float>> CImageProc::GetRelativePos() const{
    vector<vector<float>> relative_pos(28, vector<float>(2, 0.0));
    float length = 34.2;
    float width = 10.3;
    relative_pos['a' - 'a'] = {4.2, 5.7};
    relative_pos['e' - 'a'] = {7.5, 4};
    relative_pos['l' - 'a'] = {19.4, 5.7};
    relative_pos['o' - 'a'] = {18.9, 4};
    relative_pos[CAPS] = {1.7, 5.7}; //"Caps"
    relative_pos[SPACE] = {12.5, 9.5}; //"Space"
    for(int i = 0; i < relative_pos.size(); i++){
        relative_pos[i][0] /= length;
        relative_pos[i][1] /= width;
    }
    return relative_pos;
}

int CImageProc::CalcuHomography(Mat& img, Mat& tempImg){
    
    int minHessian = 200;
    Ptr<Feature2D> f2d = xfeatures2d::SURF::create(1000);
    //Ptr<Feature2D> f2d = xfeatures2d::SIFT::create(minHessian);
    //Ptr<ORB> f2d = ORB::create();

    Mat img_1 = tempImg.clone();
    Mat img_2 = img.clone();
    vector<KeyPoint> keypoints_1, keypoints_2;
    Mat descriptors_1, descriptors_2;

    f2d->detectAndCompute(img_1, Mat(), keypoints_1, descriptors_1);
    f2d->detectAndCompute(img_2, Mat(), keypoints_2, descriptors_2);
    //BFMatcher matcher;
    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(descriptors_1, descriptors_2, matches);
    //cout << "The number of matches:" << matches.size()<< endl;
    if(matches.size() == 0)
    {
        cout<<"Match Fail: No keypoints matches!"<<endl;
        return 0;
    }

    sort(matches.begin(), matches.end());
    float min_dist = matches[0].distance;
    float max_dist = matches[matches.size()-1].distance;
    //cout << "min dist=" << min_dist << endl;
    //cout << "max dist=" << max_dist << endl;
    vector<DMatch> goodMatches;
    for (int m = 0; m < matches.size(); m++)
    {
        //If using SURF, match's count should be reduced more
        // cout<<"No."<<m+1<<" DMatch distance: "<< matches[m].distance<<endl;
        if (matches[m].distance <= 2 * min_dist || (min_dist == 0 && matches[m].distance <= 30)){
            goodMatches.push_back(matches[m]);
        }
    }
    vector<Point2f> srcPoints,dstPoints;
    for(int i=0; i<goodMatches.size(); i++){
        srcPoints.push_back(keypoints_1[goodMatches[i].queryIdx].pt);
        dstPoints.push_back(keypoints_2[goodMatches[i].trainIdx].pt);
    }
    Mat img_matches;
    drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
    //imshow("SIFT match image", img_matches);


    //仿射/投影变换
    float cols = img_1.cols;
    float rows = img_1.rows;
    // Mat warp = getAffineTransform(srcPoints, dstPoints);
    //Mat homography;
    vector<unsigned char> inliersMask(srcPoints.size());
    m_homography = findHomography(srcPoints, dstPoints, cv::RANSAC, 5, inliersMask);
    
    /*
    vector<Point2f> temp_corners(4);
    temp_corners[0] = Point(0, 0);
    temp_corners[1] = Point(cols, 0 );
    temp_corners[2] = Point(cols, rows );
    temp_corners[3] = Point(0, rows );
    std::vector<Point2f> image_corners(4);
    // warpAffine(temp_corners, image_corners, warp, Size(2, 2));
    perspectiveTransform( temp_corners, image_corners, m_homography);
    for(int k=0; k<4; k++){
        line(img_matches, image_corners[k] + Point2f(cols,0),
            image_corners[(k+1)%4] + Point2f(cols,0), Scalar(0,200,0), 2);
    }
    imshow("Match image", img_matches);
    waitKey(0);*/
    return 1;
}

int CImageProc::GetButton(Mat& img, string str, vector<Point2f>& points){
    Mat TempImg = imread("../pics/keyboard.png", 0);
    if (img.empty() || !img.data) {
      return 0;
    }
    
    bool CapsOn = false;
    Display *d = XOpenDisplay((char*)0);
    if(d){
        unsigned n;
        XkbGetIndicatorState(d, XkbUseCoreKbd, &n);
        if(n&1) CapsOn = true;
    }
    
    vector<int> order;
    for(char ch : str){
        if(ch <= 'z' && ch >= 'a'){
            if(CapsOn){
                order.push_back(CAPS);
                CapsOn = false;
            }
            order.push_back(ch - 'a');
        }
        else if(ch <= 'Z' && ch >= 'A'){
            if(!CapsOn){
                order.push_back(CAPS);
                CapsOn = true;
            }
            order.push_back(ch - 'A');
        }
        else if(ch == ' '){
            order.push_back(SPACE);
        }
    }
    //saveImage(img, ObjectID);
    cvtColor(img, img, COLOR_BGR2GRAY);
    CalcuHomography(img, TempImg);
    const vector<vector<float>> RelativePos = GetRelativePos();
    vector<Point2f> TempPoints;

    float cols = TempImg.cols;
    float rows = TempImg.rows;
    for(int key : order){
        Point2f t(RelativePos[key][0] * cols, RelativePos[key][1] *rows);
        TempPoints.push_back(t);
    }
    perspectiveTransform(TempPoints, points, m_homography);
    cout <<"Number of orders: "<<points.size()<<endl;
    for(auto point : points){
        cv::circle(img, point, 3, Scalar(255), 2);
    }
    imshow("img", img);
    waitKey(0);
    return 1;
}

int CImageProc::SaveImage(Mat& img) {
    char fileName[100];
    if (img.empty() || !img.data) {
        return 0;
    }
    time_t tt = time(NULL);
    tm* t = localtime(&tt);
    sprintf(fileName, "/home/robot/exp_images/%d_%d-%d-%d.png", t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    imwrite(fileName, img);
    return 1;
}