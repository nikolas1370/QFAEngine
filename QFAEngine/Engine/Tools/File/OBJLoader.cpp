#include "OBJLoader.h"

QStaticMesh* OBJLoader::LoadModel(std::string path)
{	
	return new QStaticMesh(LoadModel(path, 0));
}


MeshData* OBJLoader::LoadModel(std::string path, unsigned int framePerSecond)
{
	MeshData* meshFrame = nullptr;// = new MeshFrames();

	int VMCount = 0;
	unsigned int* meshIndexes = nullptr;
	unsigned int rawIndexUniqueCount = 0;

	materialName materialList[101];// materialArray
	unsigned int materialListLen = 1;// first material default if face dond have material
	bool materialProcessed = false;

	std::string fileName;
	std::string pathWithNameHalf;
	std::string namesEnd;

	if (framePerSecond > 0)
	{
		fileName = Parser::GetNameFile(path.c_str());
		const char* sym = Parser::GetNextChar('_', fileName.c_str()) + 1;
		pathWithNameHalf = std::string(path.c_str(), path.length() - fileName.length() + (sym - fileName.c_str()));
		namesEnd = Parser::GetNextChar('.', fileName.c_str());
	}

	int modelFileBufferSize = 0;
	int countFrame = 0;
	std::string pathSearch;
	struct stat buf;
	for (;;)
	{
		if (framePerSecond > 0)
		{
			pathSearch.append(pathWithNameHalf);
			pathSearch.append(std::to_string(countFrame + 1).append(namesEnd));
		}
		else
			pathSearch = path;

		if (stat(pathSearch.c_str(), &buf) == -1)
			break;

		if (buf.st_size > modelFileBufferSize)
			modelFileBufferSize = buf.st_size;

		countFrame++;
		pathSearch = "";
		if (framePerSecond == 0)
			break;
	}

	modelFileBufferSize++;// for 0 in end file

	char* rawMeshInfo = nullptr;
	FVector* rawMeshVertex = nullptr;
	FVector* rawMeshNormal = nullptr;
	using UIVector = TVector<unsigned int>;
	UIVector* rawMeshIndex = nullptr;// [0] vertex position, [1] vertex normal, [2] material
	UIVector* rawMeshIndexUnique = nullptr;

	const char* chars[] =
	{
		"o ",
		"v ",
		"vn ",
		"f ",
	};

	char* modelFileBuffer = (char*)malloc(modelFileBufferSize);// in future se all file with anim and check bigest size and put in
	const int charsArraySize = sizeof(chars) / sizeof(char*);
	int charsCount[charsArraySize] = { 0 };
	std::string loadPath;
	for (int qfa = 0; qfa < countFrame; qfa++)
	{
		int modelFileLen;
		if (framePerSecond > 0)
		{
			loadPath = pathWithNameHalf;
			modelFileLen = FileLoader::load(loadPath.append(std::to_string(qfa + 1).append(namesEnd)), modelFileBuffer, modelFileBufferSize);
		}
		else
			modelFileLen = FileLoader::load(path, modelFileBuffer, modelFileBufferSize);

		modelFileBuffer[modelFileLen] = 0;
		if (!materialProcessed)
		{
			int materialNameLen = 0;
			const char* tempMaterialName = Parser::GetLine("mtllib ", modelFileBuffer, materialNameLen);
			std::string onlyPath = Parser::GetPathWithoutNameFile(path.c_str());
			std::string materialText = FileLoader::load(onlyPath.append(tempMaterialName, materialNameLen));

			char* materialTextOffset = (char*)materialText.c_str();
			for (int i = 1; ; i++)// parse material name. int i = 1 because in [0] default material
			{
				int lenMatNum = 0;
				materialTextOffset = (char*)Parser::GetLine("newmtl ", materialTextOffset, lenMatNum);
				if (!materialTextOffset)
					break;

				materialList[i].name = std::string(materialTextOffset, lenMatNum);
				materialTextOffset = (char*)Parser::GetLine("Kd ", materialTextOffset, lenMatNum);
				materialList[i].color.X = Parser::GetFloatFromString(materialTextOffset);
				materialTextOffset = (char*)Parser::GetNextChar(' ', materialTextOffset) + 1;
				materialList[i].color.Y = Parser::GetFloatFromString(materialTextOffset);
				materialTextOffset = (char*)Parser::GetNextChar(' ', materialTextOffset) + 1;
				materialList[i].color.Z = Parser::GetFloatFromString(materialTextOffset);
				materialTextOffset = (char*)Parser::GetLine("Ks ", materialTextOffset, lenMatNum);
				materialList[i].specular = Parser::GetFloatFromString(materialTextOffset);
				materialListLen++;
			}

			materialProcessed = true;
		}

		if (!rawMeshInfo)// allocate memory for templorary data
		{
			Parser::GetAmountSomeCharsInText(modelFileBuffer, modelFileLen, chars, charsCount, charsArraySize);
			int vertexSize = sizeof(FVector) * charsCount[1];
			int normalSize = sizeof(FVector) * charsCount[2];
			int indexSize = sizeof(rawIndex) * charsCount[3] * 3;// i face 3 index
			rawMeshInfo = (char*)malloc(vertexSize + normalSize + indexSize * 2);
			rawMeshVertex = (FVector*)rawMeshInfo;
			rawMeshNormal = (FVector*)&rawMeshInfo[vertexSize];
			rawMeshIndex = (UIVector*)&rawMeshInfo[vertexSize + normalSize];// [0] vertex position, [1] vertex normal, [2] material
			rawMeshIndexUnique = (UIVector*)&rawMeshInfo[vertexSize + normalSize + indexSize];
		}

		int rawVertexCount = 0;
		int rawNormalCount = 0;
		int rawIndexCount = 0;
		unsigned int selectedMaterial = 0;
		char* modelTextOffset = (char*)modelFileBuffer;
		while (true)// parse raw, vector, normal and index data
		{
			if (!modelTextOffset)
				break;

			if (Parser::CheckChars("o ", modelTextOffset))
				selectedMaterial = 0;
			else if (Parser::CheckChars("v ", modelTextOffset))
			{
				for (int i = 0; i < 3; i++)
				{
					modelTextOffset = (char*)Parser::GetNextChar(' ', modelTextOffset) + 1;
					rawMeshVertex[rawVertexCount][i] = Parser::GetFloatFromString(modelTextOffset);
				}

				rawVertexCount++;
			}
			else if (Parser::CheckChars("vn ", modelTextOffset))
			{
				for (int i = 0; i < 3; i++)
				{
					modelTextOffset = (char*)Parser::GetNextChar(' ', modelTextOffset) + 1;
					rawMeshNormal[rawNormalCount][i] = Parser::GetFloatFromString(modelTextOffset);
				}

				rawNormalCount++;
			}
			else if (Parser::CheckChars("usemtl ", modelTextOffset))
			{
				modelTextOffset = (char*)Parser::GetNextChar(' ', modelTextOffset) + 1;
				for (unsigned int i = 1; i < materialListLen + 1; i++)
				{
					if (Parser::CheckChars(materialList[i].name.c_str(), modelTextOffset, (int)materialList[i].name.length()))//materialList[i].length
					{
						selectedMaterial = i;
						break;
					}
				}
			}
			else if (Parser::CheckChars("f ", modelTextOffset))
			{
				for (size_t i = 0; i < 3; i++)
				{
					modelTextOffset = (char*)Parser::GetNextChar(' ', modelTextOffset) + 1;
					rawMeshIndex[rawIndexCount][0] = Parser::GetUIntFromString(modelTextOffset) - 1;
					modelTextOffset = (char*)Parser::GetNextChar('/', modelTextOffset) + 1;// jump over texture index
					modelTextOffset = (char*)Parser::GetNextChar('/', modelTextOffset) + 1;
					rawMeshIndex[rawIndexCount][1] = Parser::GetUIntFromString(modelTextOffset) - 1;
					rawMeshIndex[rawIndexCount][2] = selectedMaterial;
					rawIndexCount++;
				}
			}

			modelTextOffset = Parser::SkipLine(modelTextOffset);
		}

		if (!meshIndexes)
		{
			for (int i = 0; i < rawIndexCount; i++)// make unique index
			{
				bool find = false;
				for (unsigned int j = 0; j < rawIndexUniqueCount; j++)
				{
					if (rawMeshIndex[i].X == rawMeshIndexUnique[j].X && rawMeshIndex[i].Y == rawMeshIndexUnique[j].Y && rawMeshIndex[i].Z == rawMeshIndexUnique[j].Z)
					{
						find = true;
						break;
					}
				}

				if (!find)
				{
					rawMeshIndexUnique[rawIndexUniqueCount] = rawMeshIndex[i];
					rawIndexUniqueCount++;
				}
			}

			meshFrame = new MeshData(rawIndexUniqueCount, charsCount[3] * 3, materialListLen);// * 3 face have 3 index
			for (unsigned int i = 0; i < materialListLen; i++)
				meshFrame->SetMaterial(Material{ materialList[i].color, materialList[i].specular }, i);

			meshIndexes = meshFrame->GetIndexData();
			for (int i = 0; i < rawIndexCount; i++) // write not unique index
			{
				for (unsigned int j = 0; j < rawIndexUniqueCount; j++)
				{
					if (rawMeshIndex[i].X == rawMeshIndexUnique[j].X && rawMeshIndex[i].Y == rawMeshIndexUnique[j].Y && rawMeshIndex[i].Z == rawMeshIndexUnique[j].Z)
					{
						meshIndexes[i] = j;
						break;
					}
				}
			}
		}

		SSVertexMaterial* meshVertex = meshFrame->GetFrameData(qfa + 1);
		for (unsigned int i = 0; i < rawIndexUniqueCount; i++)
		{
			/*
		FVector fv = rawMeshVertex[rawMeshIndexUnique[i][0]];
		fv = fv.RotateAngleAxis(180, FVector(0, 1, 0));
		fv = fv.RotateAngleAxis(180, FVector(0, 0, -1));
		meshVertex[i].Position = fv;// rotate from blender export default forward -z, up y

		meshVertex[i].Normal.X = rawMeshNormal[rawMeshIndexUnique[i][1]].X;
		meshVertex[i].Normal.Y = rawMeshNormal[rawMeshIndexUnique[i][1]].Y * -1;
		meshVertex[i].Normal.Z = rawMeshNormal[rawMeshIndexUnique[i][1]].Z * -1;
*/
			meshVertex[i].Position = rawMeshVertex[rawMeshIndexUnique[i][0]];
			meshVertex[i].Normal = rawMeshNormal[rawMeshIndexUnique[i][1]];
			meshVertex[i].materialIndex = rawMeshIndexUnique[i][2];

		}
	}

	free(modelFileBuffer);
	free(rawMeshInfo);	
	return meshFrame;
}

