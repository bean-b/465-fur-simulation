///
///  Model.hpp
///
///
///  Created by Bret Jackson on 2/2/17.
///
///  \brief Model is used to load a 3d model file from disk. It will automatically create mesh objects uploaded to VBOs
///

#ifndef Model_hpp2
#define Model_hpp2

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/ProgressHandler.hpp>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Texture.h"
#include "GLSLProgram.h"

namespace furSim {

	typedef std::shared_ptr<class Importer> ImporterRef;

	class ProgressReporter : public Assimp::ProgressHandler
	{
	public:
		ProgressReporter();
		~ProgressReporter();
		bool Update(float percentage = -1.f);
		void reset();
	private:
		bool _firstUpdate;
	};

	class Model : public std::enable_shared_from_this<Model>
	{
	public:

		/*!
		 * Tries to load a model from disk. Scale can be used to scale the vertex locations of the model. If the model contains textures than materialColor will be ignored.
		 */
		Model(const std::string &filename, const double scale, glm::vec4 materialColor = glm::vec4(1.0));

		/*!
		 * Given a string in nff format, this will try to load a model
		 */
		Model(const std::string &fileContents, glm::vec4 materialColor = glm::vec4(1.0));
		virtual ~Model();

		virtual void draw(basicgraphics::GLSLProgram &shader);
        
        void setMaterialColor(const glm::vec4 &color);


	private:

		glm::vec4 _materialColor;

		static std::mutex _mutex;

		static thread_local Assimp::Importer _importer;
		std::unique_ptr<ProgressReporter> _reporter;
		std::vector< std::shared_ptr<furSim::Mesh> > _meshes;
		std::vector< std::shared_ptr<basicgraphics::Texture> > _textures;

		void importMesh(const std::string &filename, int &numIndices, const double scale);
		void importMeshFromString(const std::string &fileContents);
		void processNode(aiNode* node, const aiScene* scene, const glm::mat4 scaleMat);
		std::shared_ptr<furSim::Mesh> processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4 scaleMat);
		std::vector<std::shared_ptr<basicgraphics::Texture> > loadMaterialTextures(aiMaterial* mat, aiTextureType type);
	};

}

#endif /* Model_hpp */
