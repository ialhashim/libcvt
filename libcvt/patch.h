#ifndef PATCH_H
#define PATCH_H

namespace A48 {

	class Patch {
	public:

		FaceContainer fpc_;
		Vector3 center_;
		Vector3 normal_;
		Face* center_face;
		
		friend class Mesh;

		int index;

		Patch::Patch()
		{}

		Patch::~Patch()
		{
			fpc_.clear();
		}


		Vector3& center() { return center_; }
		Vector3& normal() { return normal_; }
		Face* get_center_face() { return center_face; }
		void set_center_face( Face* f ) { center_face = f; }

        FaceIter faces_patch_begin() { return fpc_.begin(); }
        FaceIter faces_patch_end() { return fpc_.end(); }
        int num_faces_patch() { return (int)fpc_.size(); }

        void set_index(int indx) {index = indx;}
        int get_index() { return index;}

		bool add_face_patch(Face* f) 
        { std::pair<FaceIter, bool> r = fpc_.insert(f); return r.second;}

		void del_face_patch(Face* f) {
			fpc_.erase(f); 
			delete(f);
		}

	};

}

#endif
