#pragma once

#include "TState.h"
#include "ofxUI.h"


namespace TFrontEndWidget
{
	enum Type
	{
		Invalid,
		PickLocal,	
	};

	BufferString<100>	ToString(TFrontEndWidget::Type Button);
	void				GetArray(ArrayBridge<TFrontEndWidget::Type>& Array);
};
SOY_DECLARE_ENUM( TFrontEndWidget );




class TStateGuiBase : public TState
{
public:
	TStateGuiBase(TApp& App);
	virtual ~TStateGuiBase()	{}

	virtual void	Update(float TimeStep);	//	update
	virtual void	Render(float TimeStep);	//	render

protected:
	virtual void	OnCanvasEvent(ofxUIEventArgs &e)=0;

protected:
	ofxUICanvas		mCanvas;
};

template<typename ENUM>
class TStateGui : public TStateGuiBase
{
public:
	TStateGui(TApp& App) :
		TStateGuiBase	( App )
	{
	}

protected:
	virtual void	OnCanvasEvent(ofxUIEventArgs &e);
	virtual ENUM	GetInvalidEnum() const=0;
	virtual void	OnButtonClick(ENUM Button)=0;
	void			CreateEnumButtons(const vec2f& ButtonSize);	//	auto-create a button for each enum entry
};





//	pick a game type (local, network, etc)
class TStatePickMode : public TStateGui<TFrontEndWidget::Type>
{
public:
	TStatePickMode(TApp& App);

protected:
	virtual TFrontEndWidget::Type	GetInvalidEnum() const						{	return TFrontEndWidget::Invalid;	}
	virtual void					OnButtonClick(TFrontEndWidget::Type Button);
};










template<typename ENUM>
void TStateGui<ENUM>::OnCanvasEvent(ofxUIEventArgs &e)
{
	if ( !e.widget )
		return;

	BufferString<100> name = e.widget->getName().c_str();
	ENUM Button = SoyEnum::ToType( name, GetInvalidEnum() );
	OnButtonClick( Button );
}


//	auto-create a button for each enum entry
template<typename ENUM>
void TStateGui<ENUM>::CreateEnumButtons(const vec2f& ButtonSize)
{
	BufferArray<ENUM,100> Enums;
	SoyEnum::GetArray( GetArrayBridge(Enums) );

	BufferString<100> EnumName = SoyEnum::GetName<ENUM>();
	mCanvas.addWidgetDown(new ofxUILabel( static_cast<const char*>(EnumName), OFX_UI_FONT_LARGE));
	mCanvas.addWidgetDown(new ofxUISpacer(0, ButtonSize.y ));

	for ( int i=0;	i<Enums.GetSize();	i++ )
	{
		auto Label = SoyEnum::ToString( Enums[i] );
		mCanvas.addWidgetDown( new ofxUIButton( static_cast<const char*>( Label ), false, ButtonSize.x, ButtonSize.y ) );
	}

}
