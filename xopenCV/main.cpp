#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Mở camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Không thể mở camera!" << std::endl;
        return -1;
    }

    // Load mô hình phát hiện mặt
    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load("/home/thuannt/x/opencv/data/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cerr << "Không thể load haarcascade!" << std::endl;
        return -1;
    }

    cv::Mat frame, gray;
    std::vector<cv::Rect> faces;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);  // tăng tương phản

        // Detect mặt
        face_cascade.detectMultiScale(gray, faces, 1.1, 4, 0, cv::Size(30, 30));

        // Vẽ khung quanh mặt
        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("Face Detection", frame);

        if (cv::waitKey(10) == 'q') break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

