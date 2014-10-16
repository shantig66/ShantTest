/*
 * CellInitHelper.cpp
 *
 *  Created on: Sep 22, 2013
 *      Author: wsun2
 */

#include "CellInitHelper.h"

CellInitHelper::CellInitHelper() {
	initInternalBdry();
}

CVector CellInitHelper::getPointGivenAngle(double currentAngle, double r,
		CVector centerPos) {
	double xPos = centerPos.x + r * cos(currentAngle);
	double yPos = centerPos.y + r * sin(currentAngle);
	return CVector(xPos, yPos, 0);
}

/**
 * Initialize data for future usage.
 * translate the intermediate data types to final input types that are acceptable by
 * initialCellsOfThreeTypes().
 * last input of initCellNodePoss is the initial position of cell nodes, which could read
 * from mesh file. must be preprocessed to have the cell center of (0,0)
 */
void CellInitHelper::initInputsFromCellInfoArray(vector<SceNodeType> &cellTypes,
		vector<uint> &numOfInitNodesOfCells,
		vector<double> &initBdryCellNodePosX,
		vector<double> &initBdryCellNodePosY,
		vector<double> &initFNMCellNodePosX,
		vector<double> &initFNMCellNodePosY, vector<double> &initMXCellNodePosX,
		vector<double> &initMXCellNodePosY, vector<CVector> &bdryNodes,
		vector<CVector> &FNMCellCenters, vector<CVector> &MXCellCenters,
		vector<CVector> &initCellNodePoss) {

	cellTypes.clear();
	numOfInitNodesOfCells.clear();
	initBdryCellNodePosX.clear();
	initBdryCellNodePosY.clear();
	initFNMCellNodePosX.clear();
	initFNMCellNodePosY.clear();
	initMXCellNodePosX.clear();
	initMXCellNodePosY.clear();

	uint FnmCellCount = FNMCellCenters.size();
	uint MxCellCount = MXCellCenters.size();

	uint maxNodePerCell =
			globalConfigVars.getConfigValue("MaxNodePerCell").toInt();
	uint initNodePerCell = initCellNodePoss.size();
	uint requiredSpaceForBdry = ceil(
			(double) bdryNodes.size() / maxNodePerCell);
	uint requiredTotalSpaceBdry = maxNodePerCell * requiredSpaceForBdry;
	initBdryCellNodePosX.resize(requiredTotalSpaceBdry, 0.0);
	initBdryCellNodePosY.resize(requiredTotalSpaceBdry, 0.0);
	initFNMCellNodePosX.resize(maxNodePerCell * FnmCellCount, 0.0);
	initFNMCellNodePosY.resize(maxNodePerCell * FnmCellCount, 0.0);
	initMXCellNodePosX.resize(maxNodePerCell * MxCellCount, 0.0);
	initMXCellNodePosY.resize(maxNodePerCell * MxCellCount, 0.0);

	for (unsigned int i = 0; i < requiredSpaceForBdry - 1; i++) {
		cellTypes.push_back(Boundary);
		numOfInitNodesOfCells.push_back(maxNodePerCell);
	}
	cout << "Required space for bdry:" << requiredSpaceForBdry << endl;
	cellTypes.push_back(Boundary);
	numOfInitNodesOfCells.push_back(
			bdryNodes.size() - (requiredSpaceForBdry - 1) * maxNodePerCell);
	cout << "after bdry calculation, size of cellType is now "
			<< cellTypes.size() << endl;

	for (uint i = 0; i < FnmCellCount; i++) {
		cellTypes.push_back(FNM);
		numOfInitNodesOfCells.push_back(initNodePerCell);
	}
	cout << "after fnm calculation, size of cellType is now "
			<< cellTypes.size() << endl;

	for (uint i = 0; i < MxCellCount; i++) {
		cellTypes.push_back(MX);
		numOfInitNodesOfCells.push_back(initNodePerCell);
	}

	cout << "after mx calculation, size of cellType is now " << cellTypes.size()
			<< endl;

	cout << "begin init bdry pos:" << endl;

	cout << "size of bdryNodes is " << bdryNodes.size() << endl;
	cout << "size of initBdryCellNodePos = " << initBdryCellNodePosX.size()
			<< endl;

	//int jj;
	//cin>>jj;

	for (uint i = 0; i < bdryNodes.size(); i++) {
		initBdryCellNodePosX[i] = bdryNodes[i].x;
		initBdryCellNodePosY[i] = bdryNodes[i].y;
		//cout << "(" << initBdryCellNodePosX[i] << "," << initBdryCellNodePosY[i]
		//		<< ")" << endl;
	}

	uint index;
	cout << "begin init fnm pos:" << endl;
	cout << "current size is" << initFNMCellNodePosX.size() << endl;
	cout << "try to resize to: " << maxNodePerCell * FnmCellCount << endl;

	cout << "size of fnm pos: " << initFNMCellNodePosX.size() << endl;

	cout.flush();

	for (uint i = 0; i < FnmCellCount; i++) {
		for (uint j = 0; j < initNodePerCell; j++) {
			index = i * maxNodePerCell + j;
			initFNMCellNodePosX[index] = FNMCellCenters[i].x
					+ initCellNodePoss[j].x;
			initFNMCellNodePosY[index] = FNMCellCenters[i].y
					+ initCellNodePoss[j].y;
			cout << "(" << initFNMCellNodePosX[index] << ","
					<< initFNMCellNodePosY[index] << ")" << endl;
		}
	}

	cout << "begin init mx pos:" << endl;

	for (uint i = 0; i < MxCellCount; i++) {
		for (uint j = 0; j < initNodePerCell; j++) {
			index = i * maxNodePerCell + j;
			initMXCellNodePosX[index] = MXCellCenters[i].x
					+ initCellNodePoss[j].x;
			initMXCellNodePosY[index] = MXCellCenters[i].y
					+ initCellNodePoss[j].y;
			cout << "(" << initMXCellNodePosX[index] << ","
					<< initMXCellNodePosY[index] << ")" << endl;
		}
	}

	cout << "finished init inputs" << endl;
}

RawDataInput CellInitHelper::generateRawInputWithProfile(
		std::vector<CVector> &cellCenterPoss, bool isInnerBdryIncluded) {
	cout << "begining of generateRawInputWithProfile" << endl;
	cout.flush();
	RawDataInput rawData;
	vector<CVector> outsideBdryNodePos;
	vector<CVector> outsideProfileNodePos;
	std::string bdryInputFileName = globalConfigVars.getConfigValue(
			"Bdry_InputFileName").toString();

	GEOMETRY::MeshGen meshGen;

	double genBdryRatio =
			globalConfigVars.getConfigValue("GenBdrySpacingRatio").toDouble();

	cout << "before calling generateMesh2DWithProfile" << endl;
	cout.flush();

	GEOMETRY::UnstructMesh2D mesh = meshGen.generateMesh2DWithProfile(
			bdryInputFileName, genBdryRatio, isInnerBdryIncluded);

	cout << "after calling generateMesh2DWithProfile" << endl;
	cout.flush();

	std::vector<GEOMETRY::Point2D> bdryPoints = mesh.getFinalBdryPts();
	cout << "breakpoint 1" << endl;
	cout.flush();
	std::vector<GEOMETRY::Point2D> profilePoints = mesh.getFinalProfilePts();
	cout << "breakpoint 2" << endl;
	cout.flush();
	for (uint i = 0; i < bdryPoints.size(); i++) {
		outsideBdryNodePos.push_back(
				CVector(bdryPoints[i].getX(), bdryPoints[i].getY(), 0));
	}
	cout << "breakpoint 3" << endl;
	cout.flush();
	rawData.bdryNodes = outsideBdryNodePos;

	for (uint i = 0; i < profilePoints.size(); i++) {
		outsideProfileNodePos.push_back(
				CVector(profilePoints[i].getX(), profilePoints[i].getY(), 0));
	}
	cout << "breakpoint 4" << endl;
	cout.flush();
	rawData.profileNodes = outsideProfileNodePos;

	// calculate average length of profile links
	double sumLength = 0;
	for (uint i = 0; i < profilePoints.size() - 1; i++) {
		CVector tmpVec = outsideProfileNodePos[i]
				- outsideProfileNodePos[i + 1];
		sumLength += tmpVec.getModul();
	}
	double aveLen = sumLength / (profilePoints.size() - 1);
	cout << "average length = " << aveLen << endl;
	//int jj;
	//cin >> jj;

	for (unsigned int i = 0; i < cellCenterPoss.size(); i++) {
		CVector centerPos = cellCenterPoss[i];
		centerPos.Print();
		if (isMXType_v2(centerPos)) {
			rawData.MXCellCenters.push_back(centerPos);
		} else {
			rawData.FNMCellCenters.push_back(centerPos);
		}

	}
	cout << "breakpoint 5" << endl;
	cout.flush();
	generateCellInitNodeInfo_v2(rawData.initCellNodePoss);
	cout << "end of generateRawInputWithProfile" << endl;
	cout.flush();
	return rawData;
}

RawDataInput CellInitHelper::generateRawInput_V2(
		std::vector<CVector>& cellCenterPoss) {
	RawDataInput baseRawInput = generateRawInputWithProfile(cellCenterPoss,
			false);

	GEOMETRY::MeshGen meshGen;
	double genBdryRatio =
			globalConfigVars.getConfigValue("GenBdrySpacingRatio").toDouble();
	std::string bdryInputFileName = globalConfigVars.getConfigValue(
			"Bdry_InputFileName").toString();
	GEOMETRY::UnstructMesh2D mesh = meshGen.generateMesh2DWithProfile(
			bdryInputFileName, genBdryRatio);
	GEOMETRY::MeshInput input = meshGen.obtainMeshInput();
	baseRawInput.cartilageData = meshGen.obtainCartilageData(mesh, input);

	std::cout << "non tip verticies size = "
			<< baseRawInput.cartilageData.nonTipVerticies.size() << std::endl;
	std::cout << " tip verticies size = "
			<< baseRawInput.cartilageData.tipVerticies.size() << std::endl;

	std::cout << " grow node 1 index = "
			<< baseRawInput.cartilageData.growNode1Index_on_tip << std::endl;
	std::cout << " grow node 2 index = "
			<< baseRawInput.cartilageData.growNode2Index_on_tip << std::endl;

	std::cout << " grow behind node 1 index = "
			<< baseRawInput.cartilageData.growNodeBehind1Index << std::endl;
	std::cout << " grow behind node 2 index = "
			<< baseRawInput.cartilageData.growNodeBehind2Index << std::endl;

	std::cout << " grow povit node 1 index = "
			<< baseRawInput.cartilageData.pivotNode1Index << std::endl;
	std::cout << " grow povit node 2 index = "
			<< baseRawInput.cartilageData.pivotNode2Index << std::endl;

	//int jj;
	//cin >> jj;

	return baseRawInput;
}

void CellInitHelper::transformRawCartData(CartilageRawData& cartRawData,
		CartPara& cartPara, std::vector<CVector>& initNodePos) {
	// step 1, switch tip node1 to pos 0
	CVector tmpPos = cartRawData.tipVerticies[0];
	cartRawData.tipVerticies[0] =
			cartRawData.tipVerticies[cartRawData.growNode1Index_on_tip];
	cartRawData.tipVerticies[cartRawData.growNode1Index_on_tip] = tmpPos;
	cartPara.growNode1Index = 0;

	// step 2, switch tip node 2 to pos 1
	tmpPos = cartRawData.tipVerticies[1];
	cartRawData.tipVerticies[1] =
			cartRawData.tipVerticies[cartRawData.growNode2Index_on_tip];
	cartRawData.tipVerticies[cartRawData.growNode2Index_on_tip] = tmpPos;
	cartPara.growNode2Index = 1;
	cartPara.tipNodeStartPos = 2;
	cartPara.tipNodeIndexEnd = cartRawData.tipVerticies.size();

	// step 3, calculate size for tip nodes
	double tipMaxExpansionRatio = globalConfigVars.getConfigValue(
			"TipMaxExpansionRatio").toDouble();
	int maxTipSize = tipMaxExpansionRatio * cartRawData.tipVerticies.size();
	cartPara.nonTipNodeStartPos = maxTipSize;
	cartPara.nodeIndexEnd = cartPara.nonTipNodeStartPos
			+ cartRawData.nonTipVerticies.size();
	cartPara.pivotNode1Index = cartPara.nonTipNodeStartPos
			+ cartRawData.pivotNode1Index;
	cartPara.pivotNode2Index = cartPara.nonTipNodeStartPos
			+ cartRawData.pivotNode2Index;
	cartPara.growNodeBehind1Index = cartPara.nonTipNodeStartPos
			+ cartRawData.growNodeBehind1Index;
	cartPara.growNodeBehind2Index = cartPara.nonTipNodeStartPos
			+ cartRawData.growNodeBehind2Index;

	// step 4, calculate size for all nodes
	double cartmaxExpRatio = globalConfigVars.getConfigValue(
			"CartMaxExpansionRatio").toDouble();
	int maxCartNodeSize = (cartRawData.tipVerticies.size()
			+ cartRawData.nonTipVerticies.size()) * cartmaxExpRatio;
	cartPara.nodeIndexTotal = maxCartNodeSize;

	// step 5, initialize the first part of initNodePos
	initNodePos.resize(cartPara.nodeIndexTotal);
	for (uint i = 0; i < cartRawData.tipVerticies.size(); i++) {
		initNodePos[i] = cartRawData.tipVerticies[i];
	}

	// step 6, initialize the second part of initNodePos
	for (uint i = 0; i < cartRawData.nonTipVerticies.size(); i++) {
		initNodePos[i + cartPara.nonTipNodeStartPos] =
				cartRawData.nonTipVerticies[i];
	}

	for (uint i = 0; i < initNodePos.size(); i++) {
		initNodePos[i].Print();
	}

	std::cout << "In cart para, grow node 1 index = " << cartPara.growNode1Index
			<< std::endl;
	std::cout << "In cart para, grow node 2 index = " << cartPara.growNode2Index
			<< std::endl;
	std::cout << "In cart para, tip node index end = "
			<< cartPara.tipNodeIndexEnd << std::endl;
	std::cout << "In cart para, grow node 1 behind index = "
			<< cartPara.growNodeBehind1Index << std::endl;
	std::cout << "In cart para, grow node 2 behind index = "
			<< cartPara.growNodeBehind2Index << std::endl;
	std::cout << "In cart para, node index end = " << cartPara.nodeIndexEnd
			<< std::endl;
	std::cout << "In cart para, node index total = " << cartPara.nodeIndexTotal
			<< std::endl;
	std::cout << "In cart para, pivot node 1 index = "
			<< cartPara.pivotNode1Index << std::endl;
	std::cout << "In cart para, pivot node 2 index = "
			<< cartPara.pivotNode2Index << std::endl;

	//int jj;
	//cin >> jj;
}

/**
 * Initialize inputs for five different components.
 */
SimulationInitData CellInitHelper::initInputsV2(RawDataInput &rawData) {

	SimulationInitData initData;

	//transformRawCartData(rawData.cartilageData, initData.cartPara,
	//		initData.cartilageNodeIsActive, initData.cartilageNodePosX,
	//		initData.cartilageNodePosY);

	uint FnmCellCount = rawData.FNMCellCenters.size();
	uint MxCellCount = rawData.MXCellCenters.size();
	uint ECMCount = rawData.ECMCenters.size();

	uint maxNodePerCell =
			globalConfigVars.getConfigValue("MaxNodePerCell").toInt();
	uint maxNodePerECM =
			globalConfigVars.getConfigValue("MaxNodePerECM").toInt();

	uint initTotalCellCount = rawData.initCellNodePoss.size();
//uint initTotalECMCount = rawData.ECMCenters.size();
	initData.initBdryCellNodePosX.resize(rawData.bdryNodes.size(), 0.0);
	initData.initBdryCellNodePosY.resize(rawData.bdryNodes.size(), 0.0);
	initData.initProfileNodePosX.resize(rawData.profileNodes.size());
	initData.initProfileNodePosY.resize(rawData.profileNodes.size());
	initData.initECMNodePosX.resize(maxNodePerECM * ECMCount);
	initData.initECMNodePosY.resize(maxNodePerECM * ECMCount);
	initData.initFNMCellNodePosX.resize(maxNodePerCell * FnmCellCount, 0.0);
	initData.initFNMCellNodePosY.resize(maxNodePerCell * FnmCellCount, 0.0);
	initData.initMXCellNodePosX.resize(maxNodePerCell * MxCellCount, 0.0);
	initData.initMXCellNodePosY.resize(maxNodePerCell * MxCellCount, 0.0);

	for (uint i = 0; i < FnmCellCount; i++) {
		initData.cellTypes.push_back(FNM);
		initData.numOfInitActiveNodesOfCells.push_back(initTotalCellCount);
	}

	cout << "after fnm calculation, size of cellType is now "
			<< initData.cellTypes.size() << endl;

	for (uint i = 0; i < MxCellCount; i++) {
		initData.cellTypes.push_back(MX);
		initData.numOfInitActiveNodesOfCells.push_back(initTotalCellCount);
	}

	cout << "after mx calculation, size of cellType is now "
			<< initData.cellTypes.size() << endl;

	cout << "begin init bdry pos:" << endl;

	cout << "size of bdryNodes is " << rawData.bdryNodes.size() << endl;
	cout << "size of initBdryCellNodePos = "
			<< initData.initBdryCellNodePosX.size() << endl;

	cout << "size of profileNodes is " << rawData.profileNodes.size() << endl;
	cout << "size of initBdryCellNodePos = "
			<< initData.initProfileNodePosX.size() << endl;
//int jj;
//cin>>jj;

	for (uint i = 0; i < rawData.bdryNodes.size(); i++) {
		initData.initBdryCellNodePosX[i] = rawData.bdryNodes[i].x;
		initData.initBdryCellNodePosY[i] = rawData.bdryNodes[i].y;
		//cout << "(" << initBdryCellNodePosX[i] << "," << initBdryCellNodePosY[i]
		//		<< ")" << endl;
	}

	for (uint i = 0; i < rawData.profileNodes.size(); i++) {
		initData.initProfileNodePosX[i] = rawData.profileNodes[i].x;
		initData.initProfileNodePosY[i] = rawData.profileNodes[i].y;
		//cout << "(" << initBdryCellNodePosX[i] << "," << initBdryCellNodePosY[i]
		//		<< ")" << endl;
	}

	uint index;
	cout << "begin init fnm pos:" << endl;
	cout << "current size is" << initData.initFNMCellNodePosX.size() << endl;
	cout << "try to resize to: " << maxNodePerCell * FnmCellCount << endl;

	cout << "size of fnm pos: " << initData.initFNMCellNodePosX.size() << endl;

	cout.flush();

	uint ECMInitNodeCount = rawData.initECMNodePoss.size();
	for (uint i = 0; i < ECMCount; i++) {
		vector<CVector> rotatedCoords = rotate2D(rawData.initECMNodePoss,
				rawData.ECMAngles[i]);
		for (uint j = 0; j < ECMInitNodeCount; j++) {
			index = i * maxNodePerECM + j;
			initData.initECMNodePosX[index] = rawData.ECMCenters[i].x
					+ rotatedCoords[j].x;
			initData.initECMNodePosY[index] = rawData.ECMCenters[i].y
					+ rotatedCoords[j].y;
		}
	}

	for (uint i = 0; i < FnmCellCount; i++) {
		for (uint j = 0; j < initTotalCellCount; j++) {
			index = i * maxNodePerCell + j;
			initData.initFNMCellNodePosX[index] = rawData.FNMCellCenters[i].x
					+ rawData.initCellNodePoss[j].x;
			initData.initFNMCellNodePosY[index] = rawData.FNMCellCenters[i].y
					+ rawData.initCellNodePoss[j].y;
			//cout << "(" << initData.initFNMCellNodePosX[index] << ","
			//		<< initData.initFNMCellNodePosY[index] << ")" << endl;
		}
	}

	for (uint i = 0; i < MxCellCount; i++) {
		for (uint j = 0; j < initTotalCellCount; j++) {
			index = i * maxNodePerCell + j;
			initData.initMXCellNodePosX[index] = rawData.MXCellCenters[i].x
					+ rawData.initCellNodePoss[j].x;
			initData.initMXCellNodePosY[index] = rawData.MXCellCenters[i].y
					+ rawData.initCellNodePoss[j].y;
			//cout << "(" << initData.initMXCellNodePosX[index] << ","
			//		<< initData.initMXCellNodePosY[index] << ")" << endl;
		}
	}
//cout << "finished init inputs, press any key to continue" << endl;

	return initData;
}

SimulationInitData_V2 CellInitHelper::initInputsV3(RawDataInput& rawData) {
	SimulationInitData_V2 initData;

	uint FnmCellCount = rawData.FNMCellCenters.size();
	uint MxCellCount = rawData.MXCellCenters.size();
	uint ECMCount = rawData.ECMCenters.size();

	uint maxNodePerCell =
			globalConfigVars.getConfigValue("MaxNodePerCell").toInt();
	uint maxNodePerECM =
			globalConfigVars.getConfigValue("MaxNodePerECM").toInt();

	uint initTotalCellCount = rawData.initCellNodePoss.size();
	//uint initTotalECMCount = rawData.ECMCenters.size();
	initData.initBdryNodeVec.resize(rawData.bdryNodes.size());
	initData.initProfileNodeVec.resize(rawData.profileNodes.size());
	transformRawCartData(rawData.cartilageData, initData.cartPara,
			initData.initCartNodeVec);
	initData.initECMNodeVec.resize(maxNodePerECM * ECMCount);
	initData.initFNMNodeVec.resize(maxNodePerCell * FnmCellCount);
	initData.initMXNodeVec.resize(maxNodePerCell * MxCellCount);

	for (uint i = 0; i < FnmCellCount; i++) {
		initData.cellTypes.push_back(FNM);
		initData.numOfInitActiveNodesOfCells.push_back(initTotalCellCount);
	}

	cout << "after fnm calculation, size of cellType is now "
			<< initData.cellTypes.size() << endl;

	for (uint i = 0; i < MxCellCount; i++) {
		initData.cellTypes.push_back(MX);
		initData.numOfInitActiveNodesOfCells.push_back(initTotalCellCount);
	}

	cout << "after mx calculation, size of cellType is now "
			<< initData.cellTypes.size() << endl;

	cout << "begin init bdry pos:" << endl;

	cout << "size of bdryNodes is " << rawData.bdryNodes.size() << endl;
	cout << "size of initBdryCellNodePos = " << initData.initBdryNodeVec.size()
			<< endl;

	cout << "size of profileNodes is " << rawData.profileNodes.size() << endl;
	cout << "size of initBdryCellNodePos = "
			<< initData.initProfileNodeVec.size() << endl;
	//int jj;
	//cin>>jj;

	for (uint i = 0; i < rawData.bdryNodes.size(); i++) {
		initData.initBdryNodeVec[i] = rawData.bdryNodes[i];
		//cout << "(" << initBdryCellNodePosX[i] << "," << initBdryCellNodePosY[i]
		//		<< ")" << endl;
	}

	for (uint i = 0; i < rawData.profileNodes.size(); i++) {
		initData.initProfileNodeVec[i] = rawData.profileNodes[i];
		//cout << "(" << initBdryCellNodePosX[i] << "," << initBdryCellNodePosY[i]
		//		<< ")" << endl;
	}

	uint index;
	cout << "begin init fnm pos:" << endl;
	cout << "current size is" << initData.initFNMNodeVec.size() << endl;
	cout << "try to resize to: " << maxNodePerCell * FnmCellCount << endl;

	cout << "size of fnm pos: " << initData.initFNMNodeVec.size() << endl;

	cout.flush();

	uint ECMInitNodeCount = rawData.initECMNodePoss.size();
	for (uint i = 0; i < ECMCount; i++) {
		vector<CVector> rotatedCoords = rotate2D(rawData.initECMNodePoss,
				rawData.ECMAngles[i]);
		for (uint j = 0; j < ECMInitNodeCount; j++) {
			index = i * maxNodePerECM + j;
			initData.initECMNodeVec[index] = rawData.ECMCenters[i]
					+ rotatedCoords[j];
		}
	}

	for (uint i = 0; i < FnmCellCount; i++) {
		for (uint j = 0; j < initTotalCellCount; j++) {
			index = i * maxNodePerCell + j;
			initData.initFNMNodeVec[index] = rawData.FNMCellCenters[i]
					+ rawData.initCellNodePoss[j];
			//cout << "(" << initData.initFNMCellNodePosX[index] << ","
			//		<< initData.initFNMCellNodePosY[index] << ")" << endl;
		}
	}

	for (uint i = 0; i < MxCellCount; i++) {
		for (uint j = 0; j < initTotalCellCount; j++) {
			index = i * maxNodePerCell + j;
			initData.initMXNodeVec[index] = rawData.MXCellCenters[i]
					+ rawData.initCellNodePoss[j];
			//cout << "(" << initData.initMXCellNodePosX[index] << ","
			//		<< initData.initMXCellNodePosY[index] << ")" << endl;
		}
	}
	//cout << "finished init inputs, press any key to continue" << endl;

	return initData;
}

vector<CVector> CellInitHelper::rotate2D(vector<CVector> &initECMNodePoss,
		double angle) {
	uint inputVectorSize = initECMNodePoss.size();
	CVector centerPosOfInitVector = CVector(0);
	for (uint i = 0; i < inputVectorSize; i++) {
		centerPosOfInitVector = centerPosOfInitVector + initECMNodePoss[i];
	}
	centerPosOfInitVector = centerPosOfInitVector / inputVectorSize;
	for (uint i = 0; i < inputVectorSize; i++) {
		initECMNodePoss[i] = initECMNodePoss[i] - centerPosOfInitVector;
	}
	vector<CVector> result;
	for (uint i = 0; i < inputVectorSize; i++) {
		CVector posNew;
		posNew.x = cos(angle) * initECMNodePoss[i].x
				- sin(angle) * initECMNodePoss[i].y;
		posNew.y = sin(angle) * initECMNodePoss[i].x
				+ cos(angle) * initECMNodePoss[i].y;
		result.push_back(posNew);
	}
	return result;
}

/**
 * Generate RawDataInput.
 */
/*
 RawDataInput CellInitHelper::generateRawInput(std::string meshInput) {

 RawDataInput rawData;
 double cellCenterInterval = globalConfigVars.getConfigValue(
 "Cell_Center_Interval").toDouble();
 double bdryNodeInterval = globalConfigVars.getConfigValue(
 "Bdry_Node_Interval").toDouble();
 double profileNodeInterval = globalConfigVars.getConfigValue(
 "Profile_Node_Interval").toDouble();
 int initNodeCountPerECM = globalConfigVars.getConfigValue(
 "InitECMNodeCount").toInt();

 initPrecisionBoundaryPoints();
 transformBoundaryPoints();
 initBoundaryLines(cellCenterInterval / 1.8);

 generateBoundaryCellNodesArray(rawData.bdryNodes, bdryNodeInterval);
 cout << "finished generating boundary cell nodes array" << endl;
 generateProfileNodesArray(rawData.profileNodes, profileNodeInterval);
 cout << "finished generating profile nodes array" << endl;
 int initProfileNodeSize = rawData.profileNodes.size();
 generateRandomAngles(rawData.ECMAngles, initProfileNodeSize);
 generateECMInitNodeInfo(rawData.initECMNodePoss, initNodeCountPerECM);
 generateCellInitNodeInfo(rawData.initCellNodePoss, meshInput);

 vector<CVector> insideCellCenters = getCellCentersInside(
 cellCenterInterval);
 cout << "INSIDE CELLS: " << insideCellCenters.size() << endl;

 for (unsigned int i = 0; i < insideCellCenters.size(); i++) {
 CVector centerPos = insideCellCenters[i];
 if (isMXType(centerPos)) {
 rawData.MXCellCenters.push_back(centerPos);
 } else {
 rawData.FNMCellCenters.push_back(centerPos);
 }
 }

 generateECMCenters(rawData.ECMCenters, insideCellCenters,
 rawData.bdryNodes);
 cout << "number of ECM: " << rawData.ECMCenters.size() << endl;

 cout << "Number of boundary nodes: " << rawData.bdryNodes.size()
 << "Number of MX cells: " << rawData.MXCellCenters.size()
 << " and number of FNM cells:" << rawData.FNMCellCenters.size()
 << endl;

 return rawData;
 }
 */

/*
 SimulationInitData CellInitHelper::generateInput(std::string meshInput) {
 RawDataInput rawData = generateRawInput(meshInput);
 return initInputsV2(rawData);
 }
 */

vector<CVector> CellInitHelper::generateCircleCentersInDisk(double diskRadius,
		double circleRadius) {
	const double PI = acos(-1.0);
	vector<CVector> result;
	CVector center(0, 0, 0);
	CVector topIncrease = CVector(circleRadius * sin(PI / 6.0),
			circleRadius * cos(PI / 6.0), 0.0);
	CVector bottomIncrease = CVector(0, 0, 0) - topIncrease;
	CVector leftIncrease = CVector(-circleRadius, 0, 0);
	CVector rightIncrease = CVector(circleRadius, 0, 0);
	CVector topIter = center;
	while (topIter.y - center.y < diskRadius) {
		CVector leftIter = topIter;
		while (Modul(leftIter - center) < diskRadius) {
			result.push_back(leftIter);
			leftIter.Print();
			leftIter = leftIter + leftIncrease;
			cout << "aaaa" << endl;
		}
		CVector rightIter = topIter + rightIncrease;
		while (Modul(rightIter - center) < diskRadius) {
			result.push_back(rightIter);
			rightIter.Print();
			rightIter = rightIter + rightIncrease;

			cout << "bbbb" << endl;
		}
		topIter = topIter + topIncrease;
	}
	CVector bottomIter = center + bottomIncrease;
	while (center.y - bottomIter.y < diskRadius) {
		CVector leftIter = bottomIter;
		while (Modul(leftIter - center) < diskRadius) {
			result.push_back(leftIter);
			leftIter.Print();
			leftIter = leftIter + leftIncrease;
			cout << "cccc" << endl;
		}
		CVector rightIter = bottomIter + rightIncrease;
		while (Modul(rightIter - center) < diskRadius) {
			result.push_back(rightIter);
			rightIter.Print();
			rightIter = rightIter + rightIncrease;
			cout << "dddd" << endl;
		}
		bottomIter = bottomIter + bottomIncrease;
	}
	cout << "eeee" << endl;
	return result;
}

vector<CVector> CellInitHelper::generateCircleCentersInDisk2(double diskRadius,
		double circleRadius) {
	const double PI = acos(-1.0);
	int sideCount = (int) (diskRadius / circleRadius);
	vector<CVector> result;
	CVector startPoint(-circleRadius * sideCount, 0, 0);
	int maxNum = 2 * sideCount + 1;
	CVector topIncrease = CVector(circleRadius * sin(PI / 6.0),
			circleRadius * cos(PI / 6.0), 0.0);
	CVector bottomIncrease = CVector(circleRadius * sin(PI / 6.0),
			-circleRadius * cos(PI / 6.0), 0.0);
	CVector rightIncrease = CVector(circleRadius, 0, 0);
	CVector topIter = startPoint;
	int topCount = 0;
	while (topCount <= sideCount) {
		int num = maxNum - topCount;
		for (int i = 0; i < num; i++) {
			CVector rightIter = topIter + i * rightIncrease;
			result.push_back(rightIter);
		}
		topCount++;
		topIter = topIter + topIncrease;
	}
	int botCount = 1;
	CVector botIter = startPoint + bottomIncrease;
	while (botCount <= sideCount) {
		int num = maxNum - botCount;
		for (int i = 0; i < num; i++) {
			CVector rightIter = botIter + i * rightIncrease;
			result.push_back(rightIter);
		}
		botCount++;
		botIter = botIter + bottomIncrease;
	}
	return result;
}

SimulationInitData CellInitHelper::generateDiskInput(std::string meshInput) {
	RawDataInput rawData = generateDiskRawInput(meshInput);
	return initInputsV2(rawData);
}

RawDataInput CellInitHelper::generateDiskRawInput(std::string meshInput) {

	RawDataInput rawData;
	vector<CVector> insideCellCenters;

	GEOMETRY::MeshGen meshGen;
	std::vector<GEOMETRY::Point2D> points = meshGen.createBdryPointsOnCircle(7,
			4);
	Criteria criteria(0.125, 3.0);
	GEOMETRY::UnstructMesh2D mesh = meshGen.generateMesh2D(points,
			GEOMETRY::MeshGen::default_list_of_seeds, criteria);
	std::vector<GEOMETRY::Point2D> centerPoints = mesh.outputTriangleCenters();
//mesh.outputVtkFile("testVTK");

	generateCellInitNodeInfo(rawData.initCellNodePoss, meshInput);

	for (unsigned int i = 0; i < centerPoints.size(); i++) {
		insideCellCenters.push_back(
				CVector(centerPoints[i].getX() + 25,
						centerPoints[i].getY() + 25, 0));
	}

	insideCellCenters = generateCircleCentersInDisk2(7, 0.999);
	for (unsigned int i = 0; i < insideCellCenters.size(); i++) {
		insideCellCenters[i] = CVector(insideCellCenters[i].GetX() + 25,
				insideCellCenters[i].GetY() + 25, 0);
	}

	cout << "INSIDE CELLS: " << insideCellCenters.size() << endl;

	for (unsigned int i = 0; i < insideCellCenters.size(); i++) {
		CVector centerPos = insideCellCenters[i];
		rawData.MXCellCenters.push_back(centerPos);
	}

	cout << "Number of boundary nodes: " << rawData.bdryNodes.size()
			<< "Number of MX cells: " << rawData.MXCellCenters.size()
			<< " and number of FNM cells:" << rawData.FNMCellCenters.size()
			<< endl;

//int jj;
//cin >> jj;

	return rawData;
}

RawDataInput CellInitHelper::generateRawInput_stab() {
	RawDataInput rawData;
	vector<CVector> insideCellCenters;
	vector<CVector> outsideBdryNodePos;
	std::string bdryInputFileName = globalConfigVars.getConfigValue(
			"Bdry_InputFileName").toString();

	GEOMETRY::MeshGen meshGen;

	GEOMETRY::UnstructMesh2D mesh = meshGen.generateMesh2DFromFile(
			bdryInputFileName);

	std::vector<GEOMETRY::Point2D> insideCenterPoints =
			mesh.getAllInsidePoints();

	double fine_Ratio =
			globalConfigVars.getConfigValue("StabBdrySpacingRatio").toDouble();

	for (uint i = 0; i < insideCenterPoints.size(); i++) {
		insideCellCenters.push_back(
				CVector(insideCenterPoints[i].getX(),
						insideCenterPoints[i].getY(), 0));
	}

	mesh = meshGen.generateMesh2DFromFile(bdryInputFileName, fine_Ratio);

//std::vector<GEOMETRY::Point2D> bdryPoints = mesh.getAllBdryPoints();
	std::vector<GEOMETRY::Point2D> bdryPoints = mesh.getOrderedBdryPts();

	for (uint i = 0; i < bdryPoints.size(); i++) {
		outsideBdryNodePos.push_back(
				CVector(bdryPoints[i].getX(), bdryPoints[i].getY(), 0));
	}

//cout << "INSIDE CELLS: " << insideCellCenters.size() << endl;

	for (unsigned int i = 0; i < insideCellCenters.size(); i++) {
		CVector centerPos = insideCellCenters[i];
		rawData.MXCellCenters.push_back(centerPos);
	}

	for (uint i = 0; i < outsideBdryNodePos.size(); i++) {
		rawData.bdryNodes.push_back(outsideBdryNodePos[i]);
	}

	generateCellInitNodeInfo_v2(rawData.initCellNodePoss);

	return rawData;
}

//TODO
void CellInitHelper::generateRandomAngles(vector<double> &randomAngles,
		int initProfileNodeSize) {
	static const double PI = acos(-1.0);
	randomAngles.clear();
	for (int i = 0; i < initProfileNodeSize; i++) {
		double randomNum = rand() / ((double) RAND_MAX + 1);
		randomAngles.push_back(randomNum * 2.0 * PI);
	}
}

void CellInitHelper::generateCellInitNodeInfo(vector<CVector> &initPos,
		std::string meshInput) {
	fstream fs;
	uint NNum, LNum, ENum;
	uint i;
	double tmp;
	std::cout << "mesh input is :" << meshInput << std::endl;
	fs.open(meshInput.c_str(), ios::in);
	if (!fs.is_open()) {
		std::string errString =
				"Unable to load mesh in string input mode, meshname: "
						+ meshInput
						+ " ,possible reason is the file is not located in the project folder \n";
		std::cout << errString;
	}
	fs >> NNum >> LNum >> ENum;
	cout << "NNum = " << NNum << std::endl;

	initPos.resize(NNum);

	for (i = 0; i < NNum; i++) {
		fs >> initPos[i].x >> initPos[i].y >> tmp;
		initPos[i].x = initPos[i].x / 20.0;
		initPos[i].y = initPos[i].y / 10.0;
		initPos[i].Print();
	}
//int jj;
//cin >> jj;
	fs.close();
}

/**
 * Initially, ECM nodes are alignd vertically.
 */
void CellInitHelper::generateECMInitNodeInfo(vector<CVector> &initECMNodePoss,
		int initNodeCountPerECM) {
	initECMNodePoss.clear();
	double ECMInitNodeInterval = globalConfigVars.getConfigValue(
			"ECM_Init_Node_Interval").toDouble();
	int numOfSegments = initNodeCountPerECM - 1;
//double totalLength = ECMInitNodeInterval * numOfSegments;
	if (numOfSegments % 2 == 0) {
		CVector initPt = CVector(0, 0, 0);
		initECMNodePoss.push_back(initPt);
		for (int i = 1; i <= numOfSegments / 2; i++) {
			CVector posSide = initPt + CVector(0, i * ECMInitNodeInterval, 0);
			CVector negSide = initPt - CVector(0, i * ECMInitNodeInterval, 0);
			initECMNodePoss.push_back(posSide);
			initECMNodePoss.push_back(negSide);
		}
	} else {
		CVector initPosPt = CVector(0, ECMInitNodeInterval / 2.0, 0);
		CVector initNegPt = CVector(0, -ECMInitNodeInterval / 2.0, 0);
		initECMNodePoss.push_back(initPosPt);
		initECMNodePoss.push_back(initNegPt);
		for (int i = 1; i <= numOfSegments / 2; i++) {
			CVector posSide = initPosPt
					+ CVector(0, i * ECMInitNodeInterval, 0);
			CVector negSide = initNegPt
					- CVector(0, i * ECMInitNodeInterval, 0);
			initECMNodePoss.push_back(posSide);
			initECMNodePoss.push_back(negSide);
		}
	}
}
//TODO
void CellInitHelper::generateECMCenters(vector<CVector> &ECMCenters,
		vector<CVector> &CellCenters, vector<CVector> &bdryNodes) {
	ECMCenters.clear();
	vector<double> angles;
	vector<CVector> vecs;
	static const double PI = acos(-1.0);

	const int numberOfECMAroundCellCenter = globalConfigVars.getConfigValue(
			"ECM_Around_Cell_Center").toInt();
	const double distFromCellCenter = globalConfigVars.getConfigValue(
			"Dist_From_Cell_Center").toDouble();
	double unitAngle = 2 * PI / numberOfECMAroundCellCenter;
	for (int i = 0; i < numberOfECMAroundCellCenter; i++) {
		angles.push_back(i * unitAngle);
		CVector vec(sin(i * unitAngle), cos(i * unitAngle), 0);
		vec = vec * distFromCellCenter;
		vecs.push_back(vec);
		//cout << "vec = (" << vec.GetX() << "," << vec.GetY() << ","
		//		<< vec.GetZ() << ")" << endl;
	}
	for (uint i = 0; i < CellCenters.size(); i++) {
		for (int j = 0; j < numberOfECMAroundCellCenter; j++) {
			CVector pos = CellCenters[i] + vecs[j];
			if (anyCellCenterTooClose(CellCenters, pos)) {
				continue;
			}
			if (anyECMCenterTooClose(ECMCenters, pos)) {
				continue;
			}
			if (anyBoundaryNodeTooClose(bdryNodes, pos)) {
				continue;
			}
			ECMCenters.push_back(pos);
			if (std::isnan(pos.GetX())) {
				throw SceException("number is NAN!", InputInitException);
			}
			//cout << "pos = (" << pos.GetX() << "," << pos.GetY() << ","
			//		<< pos.GetZ() << ")" << endl;
		}
	}
}

bool CellInitHelper::anyCellCenterTooClose(vector<CVector> &cellCenters,
		CVector position) {
	double MinDistToOtherCellCenters = globalConfigVars.getConfigValue(
			"MinDistToCellCenter").toDouble();
	int size = cellCenters.size();
	for (int i = 0; i < size; i++) {
		if (Modul(cellCenters[i] - position) < MinDistToOtherCellCenters) {
			return true;
		}
	}
	return false;
}

bool CellInitHelper::anyECMCenterTooClose(vector<CVector> &ecmCenters,
		CVector position) {
	double MinDistToOtherECMCenters = globalConfigVars.getConfigValue(
			"MinDistToECMCenter").toDouble();
	int size = ecmCenters.size();
	for (int i = 0; i < size; i++) {
		if (Modul(ecmCenters[i] - position) < MinDistToOtherECMCenters) {
			return true;
		}
	}
	return false;
}

bool CellInitHelper::anyBoundaryNodeTooClose(vector<CVector> &bdryNodes,
		CVector position) {
	double MinDistToOtherBdryNodes = globalConfigVars.getConfigValue(
			"MinDistToBdryNodes").toDouble();
	int size = bdryNodes.size();
	for (int i = 0; i < size; i++) {
		if (Modul(bdryNodes[i] - position) < MinDistToOtherBdryNodes) {
			return true;
		}
	}
	return false;
}

CellInitHelper::~CellInitHelper() {
}

std::vector<CellPlacementInfo> CellInitHelper2::obtainCentersInCircle(
		double radius, int precision, Criteria criteria) {
	std::vector<CellPlacementInfo> result;
	GEOMETRY::MeshGen meshGen;
	std::vector<GEOMETRY::Point2D> bdryPoints =
			meshGen.createBdryPointsOnCircle(radius, precision);
	GEOMETRY::UnstructMesh2D mesh = meshGen.generateMesh2D(bdryPoints);
	std::vector<GEOMETRY::Point2D> centerPoints = mesh.outputTriangleCenters();
	CellPlacementInfo info;
	for (uint i = 0; i < centerPoints.size(); i++) {
		info.centerLocation = CVector(centerPoints[i].getX(),
				centerPoints[i].getY(), 0);
		info.cellNodeType = Base;
		result.push_back(info);
	}
	return result;
}

void CellInitHelper::generateCellInitNodeInfo_v2(vector<CVector>& initPos) {
	initPos = generateInitCellNodes();
}

double CellInitHelper::getRandomNum(double min, double max) {
	double rand01 = rand() / ((double) RAND_MAX + 1);
	double randNum = min + (rand01 * (max - min));
	return randNum;
}

vector<CVector> CellInitHelper::generateInitCellNodes() {
	bool isSuccess = false;
	vector<CVector> attemptedPoss;
	while (!isSuccess) {
		attemptedPoss = attemptGeenerateInitCellNodes();
		if (isPositionQualify(attemptedPoss)) {
			isSuccess = true;
		}
	}
	return attemptedPoss;
}

vector<CVector> CellInitHelper::attemptGeenerateInitCellNodes() {
	double radius =
			globalConfigVars.getConfigValue("InitCellRadius").toDouble();
	int initCellNodeCount =
			globalConfigVars.getConfigValue("InitCellNodeCount").toInt();
	vector<CVector> poss;
	int foundCount = 0;
	double randX, randY;
	while (foundCount < initCellNodeCount) {
		bool isInCircle = false;
		while (!isInCircle) {
			randX = getRandomNum(-radius, radius);
			randY = getRandomNum(-radius, radius);
			isInCircle = (sqrt(randX * randX + randY * randY) < radius);
		}
		poss.push_back(CVector(randX, randY, 0));
		foundCount++;
	}
	return poss;
}

bool CellInitHelper::isPositionQualify(vector<CVector>& poss) {
	double minInitDist = globalConfigVars.getConfigValue(
			"MinInitDistToOtherNodes").toDouble();
	bool isQualify = true;
	for (uint i = 0; i < poss.size(); i++) {
		for (uint j = 0; j < poss.size(); j++) {
			if (i == j) {
				continue;
			} else {
				CVector distDir = poss[i] - poss[j];
				if (distDir.getModul() < minInitDist) {
					isQualify = false;
					return isQualify;
				}
			}
		}
	}
	return isQualify;
}

bool CellInitHelper::isMXType_v2(CVector position) {
	if (position.y >= internalBdryPts[0].y) {
		return false;
	}
	if (position.x >= internalBdryPts[2].x) {
		return false;
	}
	for (uint i = 0; i < internalBdryPts.size() - 1; i++) {
		CVector a = internalBdryPts[i + 1] - internalBdryPts[i];
		CVector b = position - internalBdryPts[i];
		CVector crossProduct = Cross(a, b);
		if (crossProduct.z < 0) {
			return false;
		}
	}
	return true;
}

void CellInitHelper::initInternalBdry() {
	GEOMETRY::MeshGen meshGen;
	GEOMETRY::MeshInput input = meshGen.obtainMeshInput();
	internalBdryPts = input.internalBdryPts;
}

