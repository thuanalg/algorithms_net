#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cap(0); // Mở webcam (camera 0)

    if (!cap.isOpened()) {
        std::cerr << "❌ Không thể mở camera!" << std::endl;
        return -1;
    }

    // Khởi tạo bộ phát hiện người với HOG + SVM đã huấn luyện
    cv::HOGDescriptor hog;
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

    cv::Mat frame;

    while (true) {
        cap >> frame; // Đọc frame từ webcam
        if (frame.empty()) break;

        std::vector<cv::Rect> people;
        hog.detectMultiScale(frame, people, 0, cv::Size(8,8), cv::Size(32,32), 1.05, 2);

        // Vẽ hình chữ nhật quanh người phát hiện được
        for (const auto& person : people) {
            cv::rectangle(frame, person, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("Human Detection", frame);

        // Nhấn 'q' để thoát
        if (cv::waitKey(10) == 'q') break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
