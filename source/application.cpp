#include "application.hpp"

namespace application {

   /**
    * @brief return copy of folder object
    * @param variantIndex alias or index to folder
    * @return file::CFolder or empty folder if not found
   */
   application::file::CFolder CApplication::FOLDER_Get(std::variant<std::string, uint32_t> variantIndex)
   {
      if(variantIndex.index() == 0)
      {
         for(auto it = std::begin(m_vectorFolder); it != std::end(m_vectorFolder); it++)
         {
            if(it->alias() == std::get<std::string>(variantIndex)) return *it;
         }
      }
      else
      {
         return m_vectorFolder[std::get<uint32_t>(variantIndex)];
      }

      return application::file::CFolder();
   }

}