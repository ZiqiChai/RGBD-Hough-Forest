/*
 Authors:  Nima Razavi, BIWI, ETH Zurich
           Juergen Gall, BIWI, ETH Zurich
// Email:  nrazavi@vision.ee.ethz.ch
//	   gall@vision.ee.ethz.ch

// Modified by: Ishrat Badami, AIS, Uni-Bonn
// Email:       badami@vision.rwth-aachen.de
//

// When using this software, please acknowledge the effort that
// went into development by referencing [1] and [2]:
//
// [1] Razavi N. , Gall J. and Van Gool L.
// Scalable Multiclass Object Detection
// IEEE Conference on Computer Vision and Pattern Recognition (CVPR'11), 2011.

// [2] Gall J., Yao A., Razavi N., Van Gool L., and Lempitsky V.,
// Hough Forests for Object Detection, Tracking, and Action Recognition
// IEEE Transactions on Pattern Analysis and Machine Intelligence
//

  // This code makes use of the code for [3] which follows the Microsoft Research Shared Source License Agreement.
  //
  // [3] Juergen Gall and Victor Lempitsky,
  // Class-Specific Hough Forests for Object Detection, IEE Conference on Comptuer Vision and Pattern Recognition (CVPR'09), 2009.
  //
//
// You may use, copy, reproduce, and distribute this Software for any
// non-commercial purpose, subject to the restrictions of the
// Microsoft Research Shared Source license agreement ("MSR-SSLA").
// Some purposes which can be non-commercial are teaching, academic
// research, public demonstrations and personal experimentation. You
// may also distribute this Software with books or other teaching
// materials, or publish the Software on websites, that are intended
// to teach the use of the Software for academic or other
// non-commercial purposes.
// You may not use or distribute this Software or any derivative works
// in any form for commercial purposes. Examples of commercial
// purposes would be running business operations, licensing, leasing,
// or selling the Software, distributing the Software for use with
// commercial products, using the Software in the creation or use of
// commercial products or any other activity which purpose is to
// procure a commercial gain to you or others.
// If the Software includes source code or data, you may create
// derivative works of such portions of the Software and distribute
// the modified Software for non-commercial purposes, as provided
// herein.

// THE SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO
// EXPRESS, IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT
// LIMITATION, WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A
// PARTICULAR PURPOSE, ANY WARRANTY AGAINST INTERFERENCE WITH YOUR
// ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE OR
// NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL
// FULFILL ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST
// PASS THIS DISCLAIMER ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR
// DERIVATIVE WORKS.

// NEITHER MICROSOFT NOR ANY CONTRIBUTOR TO THE SOFTWARE WILL BE
// LIABLE FOR ANY DAMAGES RELATED TO THE SOFTWARE OR THIS MSR-SSLA,
// INCLUDING DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL
// DAMAGES, TO THE MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT
// LEGAL THEORY IT IS BASED ON. ALSO, YOU MUST PASS THIS LIMITATION OF
// LIABILITY ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE
// WORKS.

*/


#define PATH_SEP "/"

#include <stdexcept>

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

#include "Detector.h"
#include "Trainer.h"


using namespace std;

// load config file for dataset
void loadConfig( string& filename, int mode,  Parameters& p ) {

    char buffer[ 1000 ];
    ifstream in( filename.c_str() );

    if( in.is_open( ) ) {

        // name of an object
        in.getline( buffer, 1000 );
        in.getline( buffer, 1000 );
        p.objectName = buffer;

        // suffix
        in.getline( buffer, 1000 );
        in.getline( buffer, 1000 );
        p.suffix = buffer;

        //        // Path to trees
        //        in.getline( buffer, 1000 );
        //        in.getline( buffer, 1000 );
        //        p.treepath = buffer;

        // test files .txt
        in.getline( buffer, 1000 );
        in.getline( buffer, 1000 );
        p.testimagefiles = buffer;

        // test image path
        in.getline( buffer, 1000 );
        in.getline( buffer, 1000 );
        p.testimagepath = buffer;

        // train file .txt
        in.getline( buffer, 1000 );
        in.getline( buffer, 1000 );
        p.trainclassfiles = buffer;

        // train image path
        in.getline( buffer, 1000 );
        in.getline( buffer, 1000 );
        p.trainclasspath = buffer;

        // Output path (candidate.txt) files
        in.getline( buffer, 1000 );
        in.getline( buffer, 1000 );
        p.outpath = buffer;

        // object model path (candidate.txt) files
        in.getline( buffer, 1000 );
        in.getline( buffer, 1000 );
        p.object_models_path = buffer;

        //        // bounding box path
        //        in.getline( buffer, 1000 );
        //        in.getline( buffer, 1000 );
        //        p.BBpath = buffer;

        // Number of trees
        in.getline( buffer, 1000 );
        in >> p.ntrees;
        in.getline( buffer, 1000 );

        // Depth of tree
        in.getline( buffer, 1000 );
        in >> p.treedepth;
        in.getline( buffer, 1000 );

        // Subset of train sequences -1: all sequences
        in.getline( buffer, 1000 );
        in >> p.subsample_images_pos;
        in >> p.subsample_images_neg;
        in.getline( buffer, 1000 );

        // Samples pixels from images
        in.getline( buffer, 1000 );
        in >> p.samples_pixel_pos;
        in >> p.samples_pixel_neg;
        in.getline( buffer, 1000 );

        // add intensity channel
        in.getline( buffer, 1000 );
        in >> p.addIntensity;
        in.getline( buffer, 1000 );

        // add surfel Channel
        in.getline( buffer, 1000 );
        in >> p.addSurfel;
        in.getline( buffer, 1000 );
        // add hog channel
        in.getline( buffer, 1000 );
        in >> p.addHoG;
        in.getline( buffer, 1000 );
        // add minmaxfilt
        in.getline( buffer, 1000 );
        in >> p.addMinMaxFilt;
        in.getline( buffer, 1000 );

        // add pose measure
        in.getline( buffer, 1000 );
        in >> p.addPoseMeasure;
        in.getline( buffer, 1000 );

        // Scales
        in.getline( buffer, 1000 );
        int size; 
        float min, max;
        in >> size;
        in >> min;
        in >> max;
        p.scales.resize(size);
        for( int i = 0; i < size ; i++ )
            p.scales[i] = float(max - min)*( i+1.f )/size;
        in.getline( buffer, 1000 );

        // smoothing kernel parameters
        in.getline( buffer, 1000 );
        in >> size;
        p.kernel_width.resize(size);
        for( int i = 0; i < size; ++i )
            in >> p.kernel_width[ i ];
        in.getline( buffer, 1000 );

        // threshold for detection
        in.getline( buffer, 1000 );
        in >> p.thresh_detection;
        in.getline( buffer, 1000 );

        // threshold for boundingBox
        in.getline( buffer, 1000 );
        in >> p.thresh_bb;
        in.getline( buffer, 1000 );

        // threshold for casting vote
        in.getline( buffer, 1000 );
        in >> p.thresh_vote;
        in.getline( buffer, 1000 );

        // mode of training: allVSBG = 0 multiClassTraining = 1 MixedMultiClassBG = 2
        in.getline( buffer, 1000 );
        in >> p.training_mode;
        in.getline( buffer, 1000 );

        // maximum number of candidates
        in.getline( buffer, 1000 );
        in >> p.max_candidates;
        in.getline( buffer, 1000 );

        // booleans
        // skip already calculated images
        in.getline( buffer, 1000 );
        in >> p.doSkip;
        in.getline( buffer, 1000 );
        // do backprojection
        in.getline( buffer, 1000 );
        in >> p.do_bpr;
        in.getline( buffer, 1000 );
        // debug on
        in.getline( buffer, 1000 );
        in >> p.DEBUG;
        in.getline( buffer, 1000 );
        // add pose info in position calculations
        in.getline( buffer, 1000 );
        in >> p.addPoseInformation;
        in.getline( buffer, 1000 );
        // add Scale info in position calculations
        in.getline( buffer, 1000 );
        in >> p.addScaleInformation;
        in.getline( buffer, 1000 );
        // addPoseScore
        in.getline( buffer, 1000 );
        in >> p.addPoseScore;
        in.getline( buffer, 1000 );


    } else {
        cerr << "Config file not found " << filename << endl;
        exit(-1);
    }
    in.close();

    switch ( mode ) {
    case 0:
        cout << endl << "------------------------------------" << endl << endl;
        cout << "Training:         " << p.objectName << endl;
        cout << "Trees:            " << p.ntrees << " " << endl;
        cout << "number of Images  " << p.subsample_images_pos << " " << p.subsample_images_neg << endl;
        cout << "number of pixels: " << p.samples_pixel_pos << " " << p.samples_pixel_neg << endl;
        cout << "Train pos:        " << p.trainclasspath << endl;
        cout << "                  " << p.trainclassfiles << endl;
        cout << endl << "------------------------------------" << endl << endl;
        break;

    case 2:
        cout << endl << "------------------------------------" << endl << endl;
        cout << "Show:             " << endl;
        cout << "Trees:            " << p.ntrees << endl;
        cout << endl << "------------------------------------" << endl << endl;
        break;

    default:
        cout << endl << "------------------------------------" << endl << endl;
        cout << "Detecting:        " << p.objectName << endl;
        cout << "Trees:            " << p.ntrees << " " << endl;
        cout << "Images:           " << p.testimagepath << endl;
        cout << "                  " << p.testimagefiles << endl;
        cout << "Scales:           ";

        for( unsigned int i = 0; i < p.scales.size(); ++i )
            cout << p.scales[ i ] << " ";
        cout << endl;

        cout << "Skipping:         " << p.doSkip << endl;
        cout << "Debugging:        " << p.DEBUG  << endl;
        cout << "Add pose info:    " << p.addPoseInformation<< endl;
        cout << endl << "------------------------------------" << endl << endl;
        break;
    }

}

// load training image filenames
void loadTrainClassFile( Parameters& p , std::vector<std::vector<string> >& vFilenames, std::vector<std::vector<CvRect> >& vBBox, std::vector<std::vector<CvPoint> >& vCenter, std::vector<vector<float> > &vPoseAngle, std::vector<vector<float> > &vPitchAngle, std::vector<string>& internal_files ) {

    ifstream in_class(p.trainclassfiles.c_str());

    if(in_class.is_open()) {

        in_class >> p.nlabels;

        vFilenames.resize(p.nlabels);
        vBBox.resize(p.nlabels);
        vCenter.resize(p.nlabels);
        vPoseAngle.resize(p.nlabels);
        vPitchAngle.resize(p.nlabels);

        p.class_structure.resize(p.nlabels);

        cout << "Classes: " << p.nlabels << endl;
        string labelfile;
        internal_files.resize(p.nlabels);

        for( int l = 0; l < p.nlabels; ++l ) {

            in_class >> p.class_structure[ l ];
            in_class >> labelfile;
            internal_files[ l ] = labelfile;

            ifstream in( labelfile.c_str() );
            if( in.is_open() ) {

                unsigned int size, dummy;
                in >> size;
                in >> dummy;

                cout << "Load Train Examples: " << l << " - " << size << endl;

                vFilenames[ l ].resize( size );
                vCenter[ l ].resize( size );
                vBBox[ l ].resize( size );
                vPoseAngle[ l ].resize( size );
                vPitchAngle[ l ].resize( size );

                for( unsigned int i = 0; i < size; ++i ) {
                    // Read filename
                    in >> vFilenames[ l ][ i ];

                    // Read bounding box
                    in >> vBBox[ l ][ i ].x;
                    in >> vBBox[ l ][ i ].y;
                    in >> vBBox[ l ][ i ].width;
                    vBBox[ l ][ i ].width -= vBBox[ l ][ i ].x;
                    in >> vBBox[ l ][ i ].height;
                    vBBox[ l ][ i ].height -= vBBox[ l ][ i ].y;

                    if(vBBox[ l ][ i ].width < 16 || vBBox[ l ][ i ].height < 16 ) {
                        cout << "Width or height are too small" << endl;
                        exit(-1);
                    }

                    // Read center points
                    in >> vCenter[ l ][ i ].x;
                    in >> vCenter[ l ][ i ].y;

                    if( p.class_structure[ l ] != 0 ) {
                        in >> vPoseAngle[l][i];
                        in >> vPitchAngle[l][i];

                    } else {

                        vPoseAngle[l][i] = NAN;
                        vPitchAngle[l][i] = NAN;
                    }
                }

            } else {
                cerr << "File not found " << labelfile.c_str() << endl;
                exit( -1 );
            }

            in.close();

        }


    } else {
        cerr << "File not found " << p.trainclassfiles.c_str() << endl;
        exit(-1);
    }

    in_class.close();

}

// load testing image filenames
void loadTestClassFile( Parameters& p, std::vector<std::vector<string> >& vFilenames ) {

    ifstream in_class(p.testimagefiles.c_str());
    if(in_class.is_open()) {
        int n_test_classes;
        in_class >> n_test_classes;
        vFilenames.resize(n_test_classes);

        cout << "number Classes: " << vFilenames.size() << endl;
        string labelfile;
        for(int l=0; l < n_test_classes; ++l) {
            in_class >> labelfile;
            ifstream in(labelfile.c_str());
            if(in.is_open()) {
                unsigned int size;
                in >> size;
                cout << "Load Test Examples: " << l << " - " << size << endl;
                vFilenames[l].resize(size);
                for(unsigned int i=0; i<size; ++i) {
                    // Read filename
                    in >> vFilenames[l][i];
                }
            } else {
                cerr << "File not found " << labelfile.c_str() << endl;
                exit(-1);
            }
            in.close();
        }
    } else {
        cerr << "File not found " << p.testimagefiles.c_str() << endl;
        exit(-1);
    }
    in_class.close();
}


void loadRawData( rawData& data, Parameters& p ) {

    // load positive file list
    loadTrainClassFile( p, data.vFilenames, data.vBBox, data.vCenter, data.vPoseAngle, data.vPitchAngle, data.internal_files );

    // generate object model
    CRForestTraining::generate3DModel( p, data.vFilenames, data.vCenter,  data.vBBox, data.vPoseAngle, data.vPitchAngle, data.cg, data.bbSize );

}

// Init and start training
void run_train( Parameters& p ) {

    // Create directories

    // create output path
    string output(p.outpath);
    string makeDir = "mkdir ";
    string execstr = makeDir + output;
    system( execstr.c_str() );

    // create forest folder for all objects
    string forest = "/forests";
    execstr += forest;
    system( execstr.c_str() );

    // create forest folder for each specific object
    string forest_object = "/FOREST_PATH_" + p.objectName + "_" + p.suffix;
    execstr += forest_object;
    system( execstr.c_str() );

    p.treepath = output + forest + forest_object;

    // save the config file into forest folder
    string command = "cp " + p.configFileName + " " + p.treepath + "/config_" + p.objectName + ".txt";
    system(command.c_str());

    // Init forest with number of trees
    CRForest crForest( p.ntrees, p.doSkip);

    if (p.doSkip && crForest.loadForest(p.treepath.c_str(), p.off_tree)) {
        return; // the forest is already trained
    }

    rawData data;
    loadRawData(data, p);

    cout << "after loading data " << p.trainclasspath << endl;

    crForest.training_mode = p.training_mode;

    // depending on the training mode you should change the class_structure
    if ( p.training_mode == 0 ) {
        std::cout<< " the class labels have kept the way they are"<< std::endl;
    } else {
        // only keep the label of the background class as 0 and the rest should just get labelled differntly
        std::cout << " the class labels have changed: the background class(with label 0) is kept and all other classes have assigned different labels according to their rank in the training file" << std::endl;
        // first check if there are only 0 and 1 in the class_structure
        bool binary = true;
        for ( unsigned int i = 0; i < p.class_structure.size() ; i++ ) {
            if ( p.class_structure[ i ] > 1 || p.class_structure[ i ] < 0 )
                binary = false;
        }
        if ( binary ) {
            int count =1;
            std::cout << " new class labels: " << std::endl;
            for ( unsigned int i=0; i < p.class_structure.size(); i++ ) {
                if ( p.class_structure[ i ] !=0 ) {
                    p.class_structure[ i ] = count;
                    count++;
                }
                std::cout << " label: " << i << " " << p.class_structure[ i ] << std::endl;
            }
        } else {
            std::cout<< " there are two classes only, training mode changed to 0 " << std::endl;
            crForest.training_mode = 0;
        }
    }

    // Train forest
    crForest.trainForest( p, data, 20, 2000);
}

void detect( Parameters& p, CRForestDetector& crDetect ) {

    std::cout << "entering detect "<< std::endl;

    // Load image names
    vector< vector< string > > vFilenames;
    loadTestClassFile(p, vFilenames);

    char buffer[3000];
    char buffer2[3000];
    char buffer3[3000];

    for ( unsigned int tcNr = 0; tcNr < vFilenames.size(); tcNr++ ) {

        // Create directory
        string test_folder(p.candidatepath);
        string test_set = vFilenames[tcNr][0];
        test_set.erase(test_set.find_first_of(PATH_SEP));
        string execstr = "mkdir " + test_folder + "/" + test_set;
        system( execstr.c_str() );
        execstr = "mkdir " + p.bbpath + "/" + test_set;
        system( execstr.c_str() );

        if ( p.select_test_set > 0 && int(tcNr) != p.select_test_set) {
            continue;
        }

        if (p.test_num < 0) { // number of test images to process
            p.file_test_num = vFilenames[tcNr].size() - p.off_test;
        } else {
            p.file_test_num = p.test_num;
        }

        // Run detector for each image
        for( unsigned int i = p.off_test; (int)i < p.off_test + p.file_test_num; ++i) {

            int pstart = clock();

            if (i >= vFilenames[tcNr].size())
                continue;

            // Creat directory for the result

            sprintf_s( buffer2, "%s/%s", (test_folder + "/").c_str(), vFilenames[tcNr][i].c_str());
            sprintf_s( buffer3, "mkdir %s", buffer2);
            system(buffer3);

            // check the files
            bool skipping = false;
            if ( p.doSkip ) {
                cout << "\n\n\ncheck the skipping"<< endl;
                skipping = true;
                // first check if the candidates.txt file exists.
                char cand_dir[3000];
                sprintf_s(cand_dir,"%s",buffer2);
                char cand_filename[3000];
                sprintf_s(cand_filename,"%s/candidates.txt",cand_dir);

                FILE* file;
                if (file = fopen(cand_filename, "r")) {
                    fclose(file);
                    // read the candidates
                    std::ifstream cand_file;
                    cand_file.open(cand_filename);
                    int cand_size =0;
                    cand_file >> cand_size;
                    cand_file.close();

                    char backpr_filename[3000];
                    sprintf_s(backpr_filename,"%s/boundingboxes.txt",cand_dir);
                    if (file = fopen(backpr_filename, "r")) {
                        fclose(file);
                        std::ifstream backpr_file;
                        backpr_file.open(backpr_filename);
                        int boxes_size =0;
                        backpr_file >> boxes_size;
                        backpr_file.close();

                        if (boxes_size != cand_size) {
                            skipping = false;
                        }
                    } else {
                        skipping = false;
                        std::cout << " cannot read boundingboxes files " << std::endl;
                    }

                } else {
                    cout << "cannot read candidate file, I proceed with running the full detection" << endl;
                    skipping = false;
                }
            }
            // the detections are already calculated
            if (skipping) {
                cout << "Skipping detection of image " << i << " of set: " << tcNr << endl;
                continue;
            }

            //////////////////////////// FROM HERE THE DETECTION STARTS///////////////////////////////////////////
            // Load image

            cv::Mat img = cv::imread(( p.testimagepath + "/" + vFilenames[ tcNr ][ i ]).c_str(),CV_LOAD_IMAGE_COLOR);
            if(img.empty()) {
                cout << "Could not load image file: " << ( p.testimagepath + "/" + vFilenames[ tcNr ][ i ] ).c_str() << endl;
                exit(-1);
            }
            else
                cout << "loaded image file: " << ( p.testimagepath + "/" + vFilenames[ tcNr ][ i ] ).c_str() << endl;

            // Load Depth Image
            string filename = vFilenames[ tcNr ][ i ];
            int size_of_string = vFilenames[ tcNr ][ i ].size();
            filename.replace( size_of_string - 4, 15, "_filleddepth.png" );
            cv::Mat depthImg = cv::imread( ( p.testimagepath + "/" + filename ).c_str(),CV_LOAD_IMAGE_ANYDEPTH );
            if( depthImg.empty() ) {

                cout << "Could not load image file: " << ( p.testimagepath + "/" + filename ).c_str() << endl;
                exit( -1 );
            }


            // preparing the variables
            int nlabels = crDetect.GetNumLabels();
            vector< float > max_heights(p.nlabels,0.0f);
            vector< float > max_widths(p.nlabels,0.0f);
            for( int l = 0 ; l < p.nlabels ; ++l ) {
                max_heights[l] = p.objectSize.second/2.f;
                max_widths[l] = p.objectSize.first/2.f;
            }

            // after reading the images and check if candidates are not already detected
            /*******************************************************************************************************************************************************************/
            //1. Assign the right clusters to them


            // extract features
            int tstart = clock();
            vector<cv::Mat> vImg;
            pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
            CRPixel::extractFeatureChannels(p, img, depthImg, vImg, normals);
            cout << "extracting feature channels\t\t" << (double)(clock() - tstart)/CLOCKS_PER_SEC << " sec" << endl;

            // 1.0 Assign the reached leaf
            tstart = clock();
            vector<cv::Mat> vImgAssign;
            crDetect.fullAssignCluster(img, depthImg, vImgAssign, vImg, normals);

            //1.1 Calculate confidance for each pixel beloging to the class
            vector<cv::Mat>  classConfidence;
            crDetect.getClassConfidence(vImgAssign, classConfidence);

            //debug
            if (p.DEBUG) {
                for (unsigned int cNr = 0; cNr < classConfidence.size(); cNr++) {

                    cv::Mat img_with_probability = cv::Mat::zeros(img.rows, img.cols, img.type());
                    for(int i = 0; i < classConfidence[cNr].rows; i++) {
                        float* pline = classConfidence[cNr].ptr<float>(i);
                        uchar* imgline = img.ptr(i);
                        uchar* pimgline = img_with_probability.ptr(i);
                        for(int j=0; j < classConfidence[cNr].cols; j++, pline++) {
                            const float p = *pline;

                            *pimgline++ =  static_cast<uchar>(0.5f * *imgline++);
                            *pimgline++ =  static_cast<uchar>((1.0f - p) * (0.5f * *imgline++) + p*100.0f);
                            *pimgline++ = static_cast<uchar>((1.0f - p) * (0.5f * *imgline++) + p*255.0f);
                        }
                    }

                    double scale = 1.5f;
                    string text;
                    if(cNr == 1) text = "background";
                    else text = p.objectName;
                    cv::putText(img_with_probability, text.c_str(), cv::Point(10, 30), 1, scale, CV_RGB(0, 255, 255), 2);

                    cv::imshow("class confidence",img_with_probability);
                    cv::waitKey(0);
                }
            }
            cout << "assignment and class confidence\t\t" << (double)(clock() - tstart)/CLOCKS_PER_SEC << " sec" << endl;

            // 1.2 vote for the center of the object class and detect the peaks as a candidate

            vector<Candidate > candidates;

            // for each class except background
            for ( int cNr = 0; cNr < nlabels - 1; cNr++) {

                int this_class = cNr;

                std::vector< Candidate > temp_candidates;

                crDetect.detectObject( img, depthImg, vImg, normals, vImgAssign, classConfidence, p, this_class, temp_candidates);

                for (unsigned int candNr = 0; candNr < temp_candidates.size(); candNr++)
                    candidates.push_back(temp_candidates[candNr]);
            }

            // 2.Sorting the candidates based on their weight


            bool end_sort= false;
            if (candidates.size() < 2)
                end_sort = true;// we do not need sorting

            while(!end_sort) {
                end_sort = true;
                for (unsigned int i = 0; i < candidates.size()-1; i++ ) {
                    if (candidates[i].weight < candidates[i+1].weight) {
                        end_sort = false;
                        Candidate cand_temp;
                        cand_temp = candidates[i];
                        candidates[i] = candidates[i+1];
                        candidates[i+1] = cand_temp;
                    }
                }
            }
            std::cout << "number of candidates " << candidates.size()  << std::endl;

            /**********************************************************************************************************************************************/

            // printing the candidate file
            sprintf_s( buffer,"%s/candidates.txt", buffer2);
            std::ofstream fp_cands;
            fp_cands.open(buffer);

            fp_cands<< candidates.size()<<std::endl;
            for (unsigned int candNr=0; candNr < candidates.size(); candNr++) {
                fp_cands << candidates[candNr].weight << " " << candidates[candNr].center.x << " " << candidates[candNr].center.y << " " << candidates[candNr].scale << " " << candidates[candNr].c << std::endl;
            }

            fp_cands << std::endl;
            fp_cands.close();

            cv::Mat copy_img;
            img.copyTo(copy_img);
            cv::Size2f img_size(img.cols, img.rows);

            // printing the bounding boxes file
            sprintf_s(buffer,"%s/boundingboxes.txt", buffer2);
            std::ofstream fp_boxes;
            fp_boxes.open(buffer);
            fp_boxes<< candidates.size()<<std::endl;

            if(candidates.size() > 0) {

                for(int cand = 0; cand< candidates.size(); cand++) {

                    // Drawing Bounding Boxs

                    std::vector< cv::Point2f > imagePoints;
                    create3DBB( p.vbbSize[candidates[cand].c], candidates[cand].coordinateSystem, img_size, imagePoints );

                    if(candidates[cand].weight > p.thresh_bb) {

                        createWireFrame (copy_img, imagePoints);
                        printScore(copy_img, p.objectName, candidates[cand].weight, candidates[cand].center, 1 );

                    }
                    for ( unsigned int ptNr = 0; ptNr < imagePoints.size(); ptNr++ )
                        fp_boxes << imagePoints[ptNr].x << " " << imagePoints[ptNr].y << " ";

                    fp_boxes << "\n";

                    fp_boxes << candidates[cand].coordinateSystem.block<1,1>(0,2) << " "
                             << candidates[cand].coordinateSystem.block<1,1>(1,2) << " "
                             << candidates[cand].coordinateSystem.block<1,1>(2,2) << " " << endl;


                    fp_boxes<< std::endl;

                }

            }

            cv::imshow("Detected object", copy_img);
            cv::waitKey(5);

            cv::imwrite(( p.bbpath + "/" + vFilenames[tcNr][i]).c_str(), copy_img);

            fp_boxes.close();
            cout << "Total Time for processing this image\t\t" << (double)(clock() - pstart)/CLOCKS_PER_SEC << " sec" << endl;
        }
    }
}

// Init and start detector
void run_detect( Parameters& p ) {

    // create treePath
    string output(p.outpath);
    string forest_object = "/forests/FOREST_PATH_" + p.objectName +"_"+ p.suffix;
    p.treepath = output + forest_object;

    string makeDir = "mkdir ";
    string statistics = output + "/statistics/" ;
    string execstr = makeDir + statistics;
    system( execstr.c_str() );
    string nodeFile = statistics + "nodes_" + p.objectName + "_" + p.suffix + ".txt";

    // Init forest with number of trees
    CRForest crForest( p.ntrees );

    // Load forest
    crForest.loadForest( p.treepath.c_str(), p.off_tree );

    // forest statistics
    ofstream out(nodeFile.c_str());
    if(out.is_open()) {

        for( int trNr = 0; trNr < crForest.vTrees.size(); trNr++) {
            for( int ndNr = 0; ndNr < crForest.vTrees[trNr]->getNumNodes(); ndNr++) {

                InternalNode *node = crForest.getNode(trNr,ndNr);
                out << node->data[ 4 ] << " " << node->depth << endl;
            }
        }
    }

    out.close();
    std::vector< int > temp_classes;
    temp_classes.resize(1);
    temp_classes[ 0 ] = -1;
    crForest.SetTrainingLabelsForDetection(temp_classes);

    // Init detector
    CRForestDetector crDetect( &crForest, p.objectSize.first, p.objectSize.second, -1.0, -1.0, p.do_bpr );
    p.nlabels = crForest.GetNumLabels();

    // create directory for detection outputs
    execstr = makeDir + output;
    system( execstr.c_str() );

    string detection = "/detection/";
    execstr += detection;
    system( execstr.c_str() );

    string detect_object =  p.objectName + "_" + p.suffix;
    execstr += detect_object;
    system( execstr.c_str() );

    p.candidatepath = output + detection + detect_object;

    // create directory for bounding box
    makeDir = "mkdir ";
    string boundingBox = "/boundingBox/";
    execstr = makeDir + p.testimagepath + boundingBox;
    system( execstr.c_str() );
    execstr += detect_object;
    system( execstr.c_str() );
    p.bbpath = p.testimagepath + boundingBox + detect_object;


    p.vbbSize.resize(p.nlabels-1);

    string filePath = p.outpath + "/object_models/" + p.objectName + ".txt";
    ifstream in(filePath.c_str());
    float buffer;
    if(in.is_open()) {

        in >>  buffer;
        in >>  buffer;

        for(int i = 0; i < 3; i++ ) {
            in >>  buffer;
            in >>  buffer;
            in >>  buffer;
        }
        in >> p.vbbSize[0].x;
        in >> p.vbbSize[0].y;
        in >> p.vbbSize[0].z;
        in.close();

    }

    // run detector
    detect(p, crDetect);
}


int main( int argc, char* argv[ ] ) {
    int mode = 1;

    // Check argument
    if( argc < 2 ) {
        cout << endl << endl << endl;
        cout << "Usage: CRForest-Detector[.exe] mode [config.txt] arguments" << std::endl;
        cout << endl << endl ;
        cout << "Training" << endl;
        cout << "  mode = 0; " << std::endl;
        cout << "  arguments: " << std::endl;
        cout << "  [tree_offset=0] [number_of_trees]" << endl << endl;
        cout << "  These parameters are for parallelization and can be ignored for serial training of trees." << endl;
        cout << "  [number_of_trees]:  the number of trees to train at this run." << endl;
        cout << "  [tree_offset]:  The offset for saving the trained trees. For only training the 7th tree: number_of_trees = 1 and tree_offset = 6" << endl;
        cout << endl << endl;

        cout << "Detection "<<endl;
        cout << "  mode = 1; " << std::endl;
        cout << "  arguments: " << std::endl;
        cout << "    [number_of_trees] [test_image_offset] [number_of_test_images] [tree_offset] [test_class] [test_set] [test_scale] " << std::endl;
        cout << endl;
        cout << "  [number_of_trees] replaces the ntrees in the config file " << endl;
        cout << "  [test_image_offset] begin detection at this image instead of the first " <<endl;
        cout << "  [number_of_test_images] run detection on this number of images instead of doing it for all images" << endl;
        cout << "  [tree_offset] loading the trees starting from this offset" << endl;
        cout << "  [test_class] running the detection only on this class" << endl;
        cout << "  [test_set] running the detection on images only in one test set" << endl;
        cout << "  [test_scale] running the detection only at this scale instead of all scales" << endl;
        cout << endl << endl << endl ;
    } else {

        Parameters param;



        std::cout << "number of arguments " << argc << endl;

        if ( argc > 1 )
            mode = atoi( argv[ 1 ] );
        else
            mode=2;

        param.off_tree = 0;

        // load configuration for dataset
        if( argc > 2 ) {
            param.configFileName = argv[ 2 ];
            loadConfig( param.configFileName, mode,  param );
        } else {
            param.configFileName = "config.txt";
            loadConfig( param.configFileName, mode, param );
        }
        cout << "in main " << param.trainclasspath << endl;
        //		float test_scale = -1;
        switch ( mode ) {

        case 0: // train forest

            if ( argc > 3 )
                param.off_tree = atoi(argv[ 3 ]);

            if ( argc > 4 )
                param.ntrees = atoi(argv[ 4 ]);
            param.scale_tree = 1.0f;

            cout << endl;
            cout<< "training mode " << param.training_mode << " ntrees " << param.ntrees << " tree_offset " << param.off_tree  <<endl;
            run_train( param );
            break;

        case 1: // detection

            cout << "running the detection" << endl;
            param.test_num = -1;
            param.off_test = 0;

            if ( argc > 3 )
                param.ntrees = atoi( argv[ 3 ] );

            if ( argc > 4 ) {
                param.off_test = atoi( argv[ 4 ] );
                param.test_num = 1;
            }

            if ( argc > 5 )
                param.test_num = atoi(argv[ 5 ]);

            param.off_tree = 0;
            if ( argc > 6 )
                param.off_tree = atoi( argv[ 6 ] );

            param.select_test_class = 0;
            if ( argc > 7 )
                param.select_test_class = atoi( argv[ 7 ] );

            param.select_test_set = -1;
            if ( argc > 8 )
                param.select_test_set = atoi( argv[ 8 ] );

            run_detect(param);
            break;

        default:
            std::cout << " The default mode is not defined " << std::endl;
            break;

        }
    }

    return 0;
}


