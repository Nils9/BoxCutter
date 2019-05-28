//#include "bcutter.h"

//Bcutter::Bcutter()
//{

//}

//std::vector<int> Bcutter::optimizeCut(std::vector<int> vbox){
//    int width = std::abs(vbox[0]-vbox[2]);
//    int height = std::abs(vbox[1]-vbox[3]);
//    float w0 = int(0.05*width);
//    float h0 = int(0.05*height);

//    //Trouver les coupes verticales optimales (y fixé)
//    int bestCut0 = NULL;
//    int bestLength = atlasSize+1;
//    for(int i = std::min(vbox[1]-h0, 0); i < vbox[1] + h0; i++){
//        int currentLength = computeLength(i, 1);
//        if((currentLength < bestLength) && currentLength > 0){
//            bestLength = currentLength;
//            bestCut0 = i;
//        }
//    }

//    int bestCut1 = NULL;
//    int bestLength = atlasSize+1;
//    for(int i = vbox[2] - h0; i < std::max(vbox[3] + h0, atlasSize[0]); i++){
//        int currentLength = computeLength(i, 0);
//        if((currentLength < bestLength) && currentLength > 0){
//            bestLength = currentLength;
//            bestCut1 = i;
//        }
//    }

//    //Trouver les coupes horizontales optimales (x fixé)
//    int bestCut2 = NULL;
//    int bestLength = atlasSize+1;
//    for(int i = std::min(vbox[2] - w0, 0); i < vbox[3] + w0; i++){
//        int currentLength = computeLength(i, 1);
//        if((currentLength < bestLength) && currentLength > 0){
//            bestLength = currentLength;
//            bestCut2 = i;
//        }
//    }

//    int bestCut3 = NULL;
//    int bestLength = atlasSize+1;
//    for(int i = vbox[4] - w0; i < std::max(vbox[4] + w0, atlasSize); i++){
//        int currentLength = computeLength(i, 1);
//        if((currentLength < bestLength) && currentLength > 0){
//            bestLength = currentLength;
//            bestCut3 = i;
//        }
//    }

//    std::vector<int> cuts;
//    cuts.push_back(bestCut0);
//    cuts.push_back(bestCut1);
//    cuts.push_back(bestCut2);
//    cuts.push_back(bestCut3);

//    return cuts;
//}

//std::vector<std::vector<int>> Bcutter::optimizedCuts(std::vector<std::vector<int>> cuts){
//    std::vector<std::vector<int>> optimizedCuts;
//    for(int i = 0; i < cuts.size(); i++){
//        std::vector<int> currentCut = optimizedCuts[i];
//        optimizedCuts.push_back(optimizeCut(currentCut));
//    }

//    return optimizedCuts;
//}

//int Bcutter::computeLength(int x, int o){
//    //à implémenter : calcule la longueur d'un coupe à x (ou y) fixé, o = 0 pour horizontal, o = 1 pour vertical
//}

//std::vector<int> Bcutter::pack(int x, int y, int o){
//    //à implémenter : pack selon la coupe c1, c2 avc l'orientation o
//    //sortie : score p/b^alpha
//}

//int Bcutter::bestEfficiency(std::vector<std::vector<int>> optimizedCuts){
//    std::vector<int> efficiency;
//    int max = 0;
//    int ind = 0
//    for(int i = 0; i < optimizedCuts.size(); i++){
//        std::vector<int> currentCut = optimizedCuts[i];
//        int scorev = pack(currentCut[0], currentCut[1], 1);
//        int scoreh = pack(currentCut[2], currentCut[3], 0);
//        if(effv > max){
//            max = scorev;
//            ind = 2*i;
//        }
//        if(effh > max){
//            max = scoreh;
//            ind = 2*i + 1;
//        }
//    }

//    return ind;
//}
