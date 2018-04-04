#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace cv::dnn;

#include <iostream>
#include <cstdlib>
using namespace std;

const size_t inWidth = 300;
const size_t inHeight = 300;
const double inScaleFactor = 1.0;
const Scalar meanVal(104.0, 177.0, 123.0);
const float confidenceThreshold = 0.5;
int main(int argc, char** argv)
{
	String modelDesc = "/home/c/cuda-workspace/opencv_tutorial/data/models/resnet/deploy.prototxt";
	String modelBinary = "/home/c/cuda-workspace/opencv_tutorial/data/models/resnet/res10_300x300_ssd_iter_140000.caffemodel";

	dnn::Net net = readNetFromCaffe(modelDesc, modelBinary);
	if (net.empty())
	{
		printf("could not load net...\n");
		return -1;
	}

	VideoCapture capture(0);
	if (!capture.isOpened()) {
		printf("could not load camera...\n");
		return -1;
	}

	Mat frame;
	int index = 0;
	while (capture.read(frame)) {
		if (frame.empty())
		{
			waitKey();
			break;
		}
		flip(frame, frame, 1);
		imshow("input", frame);
		if (frame.channels() == 4);
		cvtColor(frame, frame, COLOR_BGRA2BGR);

		Mat inputBlob = blobFromImage(frame, inScaleFactor,
			Size(inWidth, inHeight), meanVal, false, false);
		net.setInput(inputBlob, "data");

		Mat detection = net.forward("detection_out");
		vector<double> layersTimings;
		double freq = getTickFrequency() / 1000;
		double time = net.getPerfProfile(layersTimings) / freq;
		Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

		ostringstream ss;
		ss << "FPS: " << 1000 / time << " ; time: " << time << " ms";
		putText(frame, ss.str(), Point(20, 20), 0, 0.5, Scalar(0, 0, 255));
		for (int i = 0; i < detectionMat.rows; i++)
		{
			float confidence = detectionMat.at<float>(i, 2);
			if (confidence > confidenceThreshold)
			{
				int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
				int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
				int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
				int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

				Rect object((int)xLeftBottom, (int)yLeftBottom,
					(int)(xRightTop - xLeftBottom),
					(int)(yRightTop - yLeftBottom));

				rectangle(frame, object, Scalar(0, 255, 0));

				ss.str("");
				ss << confidence;
				String conf(ss.str());
				String label = "Face: " + conf;
				int baseLine = 0;
				Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
				rectangle(frame, Rect(Point(xLeftBottom, yLeftBottom - labelSize.height),
					Size(labelSize.width, labelSize.height + baseLine)),
					Scalar(255, 255, 255), CV_FILLED);
				putText(frame, label, Point(xLeftBottom, yLeftBottom),
					FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
			}
		}
		index++;
		imwrite(format("face_0%d.png", index), frame);
		imshow("dnn_face_detection", frame);
		if (waitKey(1) >= 0) break;
	}

	waitKey(0);
	return 0;
}