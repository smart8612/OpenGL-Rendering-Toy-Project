#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

glm::mat4 Model::get_model_matrix()
{
    glm::mat4 mat_model = glm::mat4(1.0f);

    mat_model = mat_model * glm::translate(glm::mat4(1.0f), vec_translate);
    mat_model = mat_model * mat_rotate;
    mat_model = mat_model * glm::scale(glm::mat4(1.0f), vec_scale);

    return mat_model;
}

bool Model::init_texture_object(std::string filepath) 
{
    int width, height, channels;
    unsigned char* image;

    stbi_set_flip_vertically_on_load(true);
    image = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb);
    if (!image)
    {
        std::cout << "failed to read image" << std::endl;
        return false;
    }

    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 
                    0, GL_RGB, GL_UNSIGNED_BYTE, image);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);        
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
    
    stbi_image_free(image);

    return true;
}

void Model::draw(int loc_a_position,  int loc_u_diffuse_texture, int loc_a_texcoord, int  loc_a_normal, int loc_u_ambient, int loc_u_diffuse, int loc_u_specular, int loc_u_shininess)
{
    glUniform1i(loc_u_diffuse_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texid);

    for (int i = 0; i < mMeshes.size(); ++i)
    {
        Material mat = mMeshes[i].mMaterial;
        
        glUniform3fv(loc_u_ambient, 1, glm::value_ptr(mat.ambient));
        glUniform3fv(loc_u_diffuse, 1, glm::value_ptr(mat.diffuse));
        glUniform3fv(loc_u_specular, 1, glm::value_ptr(mat.specular));
        glUniform1f(loc_u_shininess, mat.shininess);
        
        mMeshes[i].draw(loc_a_position, loc_a_texcoord, loc_a_normal);
    }
}

bool Model::load_model(const std::string& filename)
{
    file_name = filename;
    const aiScene* scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
    if (scene == NULL)
        return false;
    
    aiColor3D tmp;    
    aiString name;

    for (int i = 0; i < scene->mNumMeshes; ++i)
    {
        Mesh mesh;
        mesh = Mesh(scene->mMeshes[i]);
        mesh.init_buffer_objects();

        int mat_idx = scene->mMeshes[i]->mMaterialIndex;    
        
        scene->mMaterials[mat_idx]->Get(AI_MATKEY_COLOR_AMBIENT, tmp);
        glm::vec3 ambient(tmp[0], tmp[1], tmp[2]);

        scene->mMaterials[mat_idx]->Get(AI_MATKEY_COLOR_DIFFUSE, tmp);
        glm::vec3 diffuse(tmp[0], tmp[1], tmp[2]);

        scene->mMaterials[mat_idx]->Get(AI_MATKEY_COLOR_SPECULAR, tmp);
        glm::vec3 specular(tmp[0], tmp[1], tmp[2]);

        Material mat(ambient, diffuse, specular, 5.0f) ;
        scene->mMaterials[mat_idx]->Get(AI_MATKEY_NAME, name);
        mat.name = name.C_Str();

        mesh.set_material(mat);

        mMeshes.push_back(mesh);

        aiString textureFilePath;
        if (scene->mMaterials[mat_idx]->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath) == AI_SUCCESS)
        {

            size_t pos = filename.rfind("/");
            std::string basepath = filename.substr(0, pos+1);
            std::string fullpath = basepath + textureFilePath.data;

            if (!init_texture_object(fullpath))
                return false;
            
            has_texture = true;
        }

    }
    return true;
}
