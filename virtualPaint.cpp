#include<opencv2/opencv.hpp>
#include<iostream> 


cv::Mat frame;
std::vector<std::vector<int>> newPoints;

cv::Point getContours(cv::Mat imgDil)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    // cv::findContours();
    cv::findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    int area=0;
    std::vector<std::vector<cv::Point>> conpoly(contours.size());
    std::vector<cv::Rect> boundrect(contours.size());
    std::string objType{};
    cv::Point mypoint(0,0);
    for(int i=0; i< contours.size(); ++i)
    {
        area = cv::contourArea(contours[i]);
        std::cout<<"Area = "<<area<<std::endl;
        if( area > 500)
        {
            float peri = cv::arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], conpoly[i], 0.02*peri, true);
            
            std::cout<<conpoly[i].size()<<std::endl;

            boundrect[i] = cv::boundingRect(conpoly[i]);
            mypoint.x = boundrect[i].x + boundrect[i].width / 2;
            mypoint.y = boundrect[i].y;

            cv::drawContours(frame, conpoly, i , cv::Scalar(0,0,255), 3);    
            cv::rectangle(frame, boundrect[i].tl(), boundrect[i].br(),cv::Scalar(255,0,0), 5);


             
        }
    }

    return mypoint;
}


std::vector<std::vector<int>> mycolor { {124,48,117,143,170,255}, //purple
                                    { 113,46,120,179,255,255}, //red
                                     {58,61,66,90,255,255}}; //green

std::vector<cv::Scalar> mycolorVal {{255,0,255},
                                    {0,0,255},
                                    {0,255,0} };


std::vector<std::vector<int>> findColours(cv::Mat img)
{
    cv::Mat imghsv;
    cv::cvtColor(img,imghsv,cv::COLOR_BGR2HSV);

    for(int i=0; i < mycolor.size(); i++)
    {   
        cv::Scalar lower(mycolor[i][0],mycolor[i][1],mycolor[i][2]);
        cv::Scalar upper(mycolor[i][3],mycolor[i][4],mycolor[i][5]);
        cv::Mat mask;
        cv::inRange(imghsv,lower,upper,mask);
        // cv::imshow(std::to_string(i), mask);
        cv::Point mypoint = getContours(mask);
        if( mypoint.x !=0 && mypoint.y !=0 ){
            newPoints.push_back( {mypoint.x, mypoint.y,i});
        }
    }

    return newPoints;

}

void drawCanvas(std::vector<std::vector<int>> newPoints, std::vector<cv::Scalar> mycolorvalue)
{
    for( int i=0; i <newPoints.size(); ++i)
    {
        cv::circle(frame, (cv::Point(newPoints[i][0],newPoints[i][1])), 5, mycolorvalue[newPoints[i][2]],cv::FILLED );
    }

}
int main(int argc, char** argv)
{
    // cv::Mat frame;
    cv::namedWindow("virtual_paint",cv::WINDOW_AUTOSIZE);
    cv::VideoCapture cap;
    if( argc == 1)
        cap.open(0);
    else
        cap.open(argv[1]);

    if( !cap.isOpened() ) 
    {
        std::cerr<<"Cannot Capture properly\n";
        return 0;
    }

    while(true)
    {
        cap >> frame;
        
        if(frame.empty() ) break;
        cv::flip(frame, frame, 1);

        newPoints = findColours(frame);
        drawCanvas(newPoints, mycolorVal);
        cv::imshow("virtual_paint",frame);     
        char c = static_cast<char>(cv::waitKey(1) );
        if( c == 27)
            break;
    }

    cv::destroyAllWindows();



    return 0;
}