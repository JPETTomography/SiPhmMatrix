#ifndef ___DISPL___ROOT___
#	define ___DISPL___ROOT___
#include <TCanvas.h>
#include <string>
template<class RootObject>
void DisplayObject(std::string filename,RootObject* obj, std::string drawp="", uint szx=800,uint szy=600){
	TCanvas canvas("","",szx,szy);
	obj->Draw(drawp.c_str());
	canvas.SaveAs(filename.c_str());
	delete obj;
}
template<class RootObject, typename addidional>
void DisplayObject_plus(std::string filename,RootObject* obj, std::string drawp, addidional action, uint szx=800,uint szy=600){
	TCanvas canvas("","",szx,szy);
	obj->Draw(drawp.c_str());
	action(obj);
	canvas.SaveAs(filename.c_str());
	delete obj;
}
template<class RootObject, typename actbefore, typename actafter>
void DisplayObject_plusplus(std::string filename,actbefore before,RootObject* obj, std::string drawp, actafter after, uint szx=800,uint szy=600){
	TCanvas canvas("","",szx,szy);
	before(&canvas,obj);
	obj->Draw(drawp.c_str());
	after(&canvas,obj);
	canvas.SaveAs(filename.c_str());
	delete obj;
}
#endif
