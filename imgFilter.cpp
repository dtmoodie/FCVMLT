#include "imgfilter.h"

imgFilter::imgFilter():
    numThreads(8)
{

}


imgFilter::imgFilter(filterType type_):
    numThreads(8)
{
    type = type_;
    if(type == threshold)
    {
        parameters.resize(2);
        parameters[0] = param(t_char,0,"Lower");
        parameters[1] = param(t_char,255,"Upper");
        filterName = "Threshold";
        numParameters = 2;
    }
    if(type == sobel)
    {
        parameters.resize(5);
        parameters[0] = param(t_int,1,"Scale");
        parameters[1] = param(t_int,0,"Delta");
        parameters[2] = param(t_int,3,"Depth");
        parameters[3] = param(t_int,3,"Kernel Size");
        parameters[4] = param(t_char,0,"Direction");
        numParameters = 5;
        filterName = "Sobel";
    }
    if(type == smooth)
    {
        filterName = "Smooth";
        numParameters= 5;
        parameters.resize(numParameters);
        QStringList types;
        types << "Gaussian";
        types << "Blur No Scale";
        types << "Blur";
        types << "Median";
        types << "Bilateral";
        types << "Adaptive Bilateral";
        parameters[0] = param(t_pullDown,0,"Smooth type",types);
        parameters[1] = param(t_int, 3, "Size 1");
        parameters[2] = param(t_int, 3, "Size 2");
        parameters[3] = param(t_double, 0, "Sigma 1");
        parameters[4] = param(t_double,0,"Sigma 2");
    }
    if(type == gabor)
    {
        numParameters = 7;
        parameters.resize(numParameters);
        param ksize;
        ksize.name = "Kernel Size";
        ksize.type = t_int;
        ksize.value = 5;
        parameters[0] = ksize;
        param sigma;
        sigma.name = "Sigma";
        sigma.type = t_double;
        sigma.value = 1;
        parameters[1] = sigma;
        param theta;
        theta.name = "Theta";
        theta.value = 0;
        theta.type = t_double;
        parameters[2] = theta;
        param lambda;
        lambda.name = "Lambda";
        lambda.type = t_double;
        lambda.value = 0;
        parameters[3] = lambda;
        param gamma;
        gamma.name = "Gamma";
        gamma.type = t_double;
        gamma.value = 0;
        parameters[4] = gamma;
        param psi;
        psi.name = "Psi";
        psi.type = t_double;
        psi.value = 3.14159;
        parameters[5] = psi;
        param ktype;
        ktype.name = "Filter coefficient";
        ktype.type = t_int;
        ktype.value = CV_64F;
        parameters[6] = ktype;
        filterName = "Gabor Filter";
    }
    if(type == resize)
    {
        numParameters = 2;
        parameters.resize(2);
        param sizeX;
        sizeX.name = "Width";
        sizeX.type = t_int;
        sizeX.value = 640;
        parameters[0] = sizeX;
        param sizeY;
        sizeY.name = "Height";
        sizeY.type = t_int;
        sizeY.value = 480;
        parameters[1] = sizeY;
        filterName = "Resize";
    }
    if(type == crop)
    {
        numParameters = 4;
        parameters.resize(numParameters);
        param top;
        top.name = "Left";
        top.type = t_int;
        top.value = 0;
        parameters[0] = top;
        param bot;
        bot.name = "Top";
        bot.type = t_int;
        bot.value = 0;
        parameters[1] = bot;
        param left;
        left.name = "Width";
        left.type = t_int;
        left.value = 640;
        parameters[2] = left;
        param right;
        right.name = "Height";
        right.type = t_int;
        right.value = 480;
        parameters[3] = right;
        filterName = "Crop";
    }
    if(type == grey)
    {
        numParameters = 0;
        parameters.resize(numParameters);
        filterName = "Convert to Grey";
    }
    if(type == laplacian)
    {
        filterName = "Laplacian";
        numParameters = 3;
        parameters.resize(numParameters);
        param scale;
        scale.name = "Scale";
        scale.type = t_int;
        scale.value = 1;
        parameters[0] = scale;
        param delta;
        delta.name = "Delta";
        delta.type = t_int;
        delta.value = 0;
        parameters[1] = delta;
        param ksize;
        ksize.name = "Kernel Size";
        ksize.type = t_int;
        ksize.value = 3;
        parameters[2] = ksize;
    }
    if(type == pyrMeanShift)
    {
        filterName = "Pyramid Mean Shift";
        numParameters = 3;
        parameters.resize(numParameters);
        parameters[0] = param(t_double, 3, "Spatial Window Radius");
        parameters[1] = param(t_double, 3, "Color Window Radius");
        parameters[2] = param(t_int, 3, "Max level");
    }
    if(type == scharr)
    {
        filterName = "Sharr Filter";
        numParameters = 4;
        parameters.resize(numParameters);
        parameters[0] = param(t_int,1, "Dx Order");
        parameters[1] = param(t_int,1,"Dy Order");
        parameters[2] = param(t_double,1,"Scale");
        parameters[3] = param(t_double,0,"Delta");
    }
    if(type == canny)
    {
        filterName = "Canny Edge Detect";
        numParameters = 3;
        parameters.resize(numParameters);
        parameters[0] = param(t_int, 50, "Low Threshold");
        parameters[1] = param(t_int, 100, "High Threshold");
        parameters[2] = param(t_int, 3, "Kernel Size");
    }
    if(type == gradientOrientation)
    {
        parameters.resize(4);
        parameters[0] = param(t_int,1,"Scale");
        parameters[1] = param(t_int,0,"Delta");
        parameters[2] = param(t_int,3,"Kernel Size");
        parameters[3] = param(t_char,0,"Direction");
        numParameters = 5;
        filterName = "Gradient Orientation";
    }

}

cv::Mat imgFilter::applyFilter(cv::Mat img)
{
    if(img.empty())
    {
        cv::Exception e;
        e.msg = "Attempted to filter an empty image!";
        e.err = e.msg;
        throw e;
    }
    cv::Mat output;
    if(type == threshold)
    {
        cv::Mat greater = img > parameters[0].value;
        cv::Mat less = img < parameters[1].value;
        output = greater.mul(less);
    }
    if(type == sobel)
    {
        cv::Mat grad_x, grad_y;
        if(parameters[4].value == 0 || parameters[4].value == 2)
        {
            try
            {
                cv::Sobel(img,grad_x,CV_32F, 1,0,parameters[3].value,parameters[0].value, parameters[1].value, cv::BORDER_DEFAULT);
            }catch(std::exception &e)
            {
                std::cout << e.what() << std::endl;
                return output;
            }
        }
        if(parameters[4].value == 1 || parameters[4].value == 2)
        {
            try
            {
                cv::Sobel(img,grad_y,CV_32F, 0,1,parameters[3].value,parameters[0].value, parameters[1].value, cv::BORDER_DEFAULT);
            }catch(std::exception &e)
            {
                std::cout << e.what() << std::endl;
                return output;
            }
        }
        if(parameters[4].value == 0)
        {
                output = grad_x;
        }
        if(parameters[4].value == 1)
        {
            output = grad_y;
        }
        if(parameters[4].value == 2)
        {
            cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, output);
        }
    }
    if(type == smooth)
    {
        if(!((int)parameters[1].value % 2)) return output;
        if(!((int)parameters[2].value % 2)) return output;
        if(parameters[0].value == 0)
        {
            // Gaussian
            try
            {
                cv::GaussianBlur(img,output,cv::Size(parameters[1].value,parameters[2].value),parameters[3].value,parameters[4].value);
            }catch(cv::Exception &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        if(parameters[0].value == 1)
        {
            // Blur no scale
            try
            {
                cv::blur(img,output,cv::Size(parameters[1].value,parameters[2].value));
            }catch(cv::Exception &e)
            {
                std::cout << e.what();
            }
        }
        if(parameters[0].value == 2)
        {
            // Blur
            try
            {
                cv::blur(img,output,cv::Size(parameters[1].value,parameters[2].value));
            }catch(cv::Exception &e)
            {
                std::cout << e.what();
            }
        }
        if(parameters[0].value == 3)
        {
            // Median
            try
            {
                cv::medianBlur(img,output,parameters[1].value);
            }catch(cv::Exception &e)
            {
                std::cout << e.what();
            }

        }
        if(parameters[0].value == 4)
        {
            // Bilateral
            try
            {
              cv::bilateralFilter(img,output,parameters[1].value,parameters[3].value,parameters[4].value);
            }catch(cv::Exception &e)
            {
                std::cout << e.what();
            }
        }
        if(parameters[0].value == 5)
        {
            try
            {
                //cv::adaptiveBilateralFilter(img,output,cv::Size(parameters[1].value,parameters[2].value),parameters[3].value);
            }catch(cv::Exception &e)
            {
                std::cout << e.what();
            }
        }
    }
    if(type == gabor)
    {
        cv::Mat filter = cv::getGaborKernel(cv::Size(parameters[0].value,parameters[0].value),
                parameters[1].value,parameters[2].value,parameters[3].value,parameters[4].value,parameters[5].value);
        cv::Mat grey = img;
        if(img.type() == CV_8UC3)
            cv::cvtColor(grey,grey,CV_BGR2GRAY);
        cv::filter2D(grey,output,CV_32F,filter);
        cv::convertScaleAbs(output, output);
        output.convertTo(output,CV_8U,255);
    }

    if(type == resize)
    {
        cv::resize(img,output, cv::Size(parameters[0].value,parameters[1].value));
    }
    if(type == crop)
    {

        cv::Rect ROI(parameters[0].value,parameters[1].value,
                parameters[2].value,parameters[3].value);
        try
        {
            output = img(ROI);
        }catch(cv::Exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    if(type == grey)
    {
        if(img.channels() == 1)
        {
            std::cout << "Img already grey!";
            output = img;
        }else
        {
            cv::cvtColor(img,output,CV_RGB2GRAY);
            std::cout << output.type() << std::endl;
        }
    }
    if(type == laplacian)
    {
        try
        {
            cv::Laplacian(img,output, CV_16S, parameters[2].value, parameters[0].value, parameters[1].value);
            cv::convertScaleAbs(output,output);
        }
        catch(cv::Exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    if(type == pyrMeanShift)
    {
        cv::pyrMeanShiftFiltering(img,output,parameters[0].value,parameters[1].value,parameters[2].value);
    }
    if(type == scharr)
    {
        cv::Scharr(img,output,CV_32S, parameters[0].value,parameters[1].value,parameters[2].value,parameters[3].value);
    }
    if(type == canny)
    {
        if(((int)parameters[2].value) %2)
        {
            if(img.channels() != 1)
            {
                std::cout << "Can only perform canny on a single channel" << std::endl;
                cv::cvtColor(img,img,CV_RGB2GRAY);
            }
            try
            {
                cv::Canny(img,output,parameters[0].value,parameters[1].value,parameters[2].value);
            }catch(cv::Exception &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }
    if(type == gradientOrientation)
    {
        if(img.channels() == 3)
        {
            cv::cvtColor(img,img,CV_BGR2GRAY);
        }
        cv::Mat gradX, gradY;
        cv::Sobel(img,gradX,CV_32F,1,0,parameters[2].value, parameters[0].value,parameters[1].value, cv::BORDER_DEFAULT);
        cv::Sobel(img,gradY,CV_32F,0,1,parameters[2].value, parameters[0].value,parameters[1].value, cv::BORDER_DEFAULT);
        output = cv::Mat::zeros(img.rows,img.cols,CV_32F);
        if(procThreads.size() != numThreads)
            procThreads.resize(numThreads);
        for(int i = 0; i < numThreads; ++i)
        {
            procThreads[i].reset(new boost::thread(boost::bind(&imgFilter::gradientOrientationHelper,this,gradX,gradY,output,i)));
        }
        for(int i = 0; i < numThreads; ++i)
        {
            procThreads[i]->join();
        }
    }
    return output;
}

void imgFilter::gradientOrientationHelper(cv::Mat gradX, cv::Mat gradY, cv::Mat orientation, int threadNum)
{
    for(int i = threadNum; i < orientation.rows * orientation.cols; i += numThreads)
    {
        ((float*)orientation.data)[i] = atan2(((float*)gradY.data)[i],((float*)gradX.data)[i]);
    }
}


// Statistics extraction
featureExtractor::featureExtractor()
{

}

featureExtractor::featureExtractor(statType type_):
    type(type_)
{
    if(type == sum)
    {
        name = "Sum";
        numFeatures = 1;
    }
    if(type == avg)
    {
        name = "Avg";
        numFeatures = 1;
    }
    if(type == median)
    {
        name = "Median";
        numFeatures = 1;
    }
    if(type == stdev)
    {
        name = "Standard Deviation";
        numFeatures = 1;
    }
    if(type == hist)
    {
        name = "Histogram";
        numFeatures = 10;
        parameters.resize(6);
        parameters[0] = param(t_int, 10, "Num bins");
        parameters[1] = param(t_double, 0, "Bottom of Range");
        parameters[2] = param(t_double, 255, "Top of Range");
        parameters[3] = param(t_int, 1, "Uniform bin sizes");
        parameters[4] = param(t_int, 0, "Accumulate histograms");
        parameters[5] = param(t_bool, 1, "Auto select range");
    }
    if(type == sift)
    {
        name = "Scale Invariant Feature Transform";
        numFeatures = 128;
    }
    if(type == HoG)
    {
        name = "Histogram of Gradients";
        numFeatures = 1;
    }
    if(type == orb)
    {
        name = "ORB";
        numFeatures = 1;
    }
}

bool featureExtractor::extractFeatures(cv::Mat src, cv::Mat& features)
{
    if(type == sum)
    {
        cv::Scalar output = cv::sum(src);
        for(int i = 0; i < src.channels(); ++i)
        {
            ((float*)features.data)[i] = output[i];
        }
    }
    if(type == avg)
    {
        cv::Scalar output = cv::mean(src);
        for(int i = 0; i < src.channels(); ++i)
        {
            ((float*)features.data)[i] = output[i];
        }
    }
    if(type == median)
    {

    }
    if(type == stdev)
    {/*
        cv::Mat stddev;
        cv::meanStdDev(src,cv::Mat(), stddev);
        for(unsigned int i = 0; i < src.channels(); ++i)
        {
            features.at<float>(i) = stddev.at<float>(i);
        }*/
    }
    if(type == hist)
    {
        std::vector<cv::Mat> channels;
        cv::split(src, channels);
        float range[2];
        if(parameters[5].value == 1)
        {
            double minVal, maxVal;
            cv::minMaxLoc(src, &minVal, &maxVal);
            range[0] = (float)minVal;
            range[1] = (float)maxVal;
            parameters[1].value = minVal;
            parameters[2].value = maxVal;
        }else
        {
            range[0] = (float)parameters[1].value;
            range[1] = (float)parameters[2].value;
        }
        const float* histRange = {range};
        int histSize = parameters[0].value;
        histPlanes.resize(src.channels());
        for(int i = 0; i < src.channels(); ++i)
        {
            cv::calcHist(&channels[i], 1, 0, cv::Mat(), histPlanes[i], 1, &histSize, &histRange, parameters[3].value, parameters[4].value);
        }

        for(int i = 0; i < src.channels(); ++i)
        {
            for(int j = 0; j < histPlanes[i].rows; ++j)
            {
                float val = histPlanes[i].at<float>(j);

                if(features.channels() == 3)
                    features.at<cv::Vec3f>(0,j)[i] = val;
                else
                    features.at<float>(0,j) = val;
            }
        }
    }
    if(type == sift)
    {

    }
    if(type == HoG)
    {

    }
    if(type == orb)
    {

    }
    return true;
}
void featureExtractor::recalculateNumParams()
{
    if(type == hist)
    {
        numFeatures = parameters[0].value;
    }
}

featureWindow::featureWindow()
{

}

featureWindow::featureWindow(statMethod method_, featureExtractor** curExtractor_):
    method(method_),
    curExtractor(curExtractor_)
{
    if(method == wholeImage)
    {
        statName = "Whole Image";
    }
    if(method == ROI)
    {
        parameters.resize(4);
        parameters[0] = param(t_int, 0, "Left");
        parameters[1] = param(t_int, 0, "Top");
        parameters[2] = param(t_int, 640, "Width");
        parameters[3] = param(t_int, 480, "Height");
        statName = "Region of Interest";
    }
    if(method == superPixel)
    {
        parameters.resize(4);
        parameters[0] = param(t_int, 4, "Super Pixel Width");
        parameters[1] = param(t_int, 4, "Super Pixel Height");
        parameters[2] = param(t_int, 4, "Super Pixel Step X");
        parameters[3] = param(t_int, 4, "Super Pixel Step Y");
        statName = "Super Pixel";
    }
    if(method == perPixel)
    {
        statName = "Per Pixel";
    }
    if(method == keyPoint)
    {
        statName = "Key Points";
    }
}

cv::Mat featureWindow::extractFeatures(cv::Mat src, bool &isImg)
{
    isImg = false;
    cv::Mat output;
    if(src.empty()) return output;
    if(method == wholeImage)
    {
        // Need to initialize the size of output inside this function so that it can be done once
        output = cv::Mat::zeros(1,(*curExtractor)->numFeatures * src.channels(),CV_32F);
        (*curExtractor)->extractFeatures(src, output);
    }
    if(method == ROI)
    {
        src = src(cv::Rect(parameters[0].value, parameters[1].value, parameters[2].value, parameters[3].value));
        output = cv::Mat::zeros(1,(*curExtractor)->numFeatures * src.channels(), CV_32F);
        (*curExtractor)->extractFeatures(src, output);
    }
    if(method == superPixel)
    {
        int stepsX = (src.cols - parameters[0].value) / parameters[2].value;
        int stepsY = (src.rows - parameters[1].value) / parameters[3].value;
        // 0 - width, 1 - height, 2 - stepX, 3 - step y
        if(parameters[0].value == -1)
        {
            // Calculate strips along the width of the image
            stepsX = 1;
            parameters[0].value = src.cols;
        }
        if(parameters[1].value == -1)
        {
            // Calculate strips along the height of the image
            stepsY = 1;
            parameters[1].value = src.rows;
        }
        if(src.channels() == 1)
            output = cv::Mat::zeros(stepsY,stepsX*(*curExtractor)->numFeatures,CV_32F);
        else
            output = cv::Mat::zeros(stepsY,stepsX*(*curExtractor)->numFeatures,CV_32FC3);
        for(int i = 0; i < stepsY; ++i)
        {
            for(int j = 0; j < stepsX; ++j)
            {
                if(j * parameters[2].value + parameters[0].value > src.cols)
                    break;
                if(i * parameters[3].value + parameters[1].value > src.rows)
                    break;
                //std::cout << j * parameters[2].value + parameters[0].value << " "
                //          << i * parameters[3].value + parameters[1].value << std::endl;
                cv::Mat roi = src(cv::Rect(j * parameters[2].value,
                                           i * parameters[3].value,
                                            parameters[0].value,
                                            parameters[1].value));
                (*curExtractor)->extractFeatures(roi, output(cv::Rect(j * (*curExtractor)->numFeatures,
                                                                      i,
                                                                      (*curExtractor)->numFeatures,
                                                                      1)));
            }
        }
        // Now build an image to represent what was extracted, if possible
        if((*curExtractor)->numFeatures == 1)
        {
            output.reshape(src.channels(),stepsY);
            isImg = true;
        }
    }
    if(method == perPixel)
    {

    }
    if(method == keyPoint)
    {

    }
    return output;
}


cv::Mat featureWindow::extractFeatures(cv::Mat src, bool &isImg, featureExtractor **curExtractor_)
{
    curExtractor = curExtractor_;
    return extractFeatures(src,isImg);
}
