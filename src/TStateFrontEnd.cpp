#include "TStateFrontEnd.h"
#include "TGame.h"
#include "TApp.h"



BufferString<100> TFrontEndWidget::ToString(TFrontEndWidget::Type Button)
{
	switch( Button )
	{ 
	default:
	case Invalid:	return "Invalid";
	case PickLocal:	return "Local";
	}
}

void TFrontEndWidget::GetArray(ArrayBridge<TFrontEndWidget::Type>& Array)
{
	//Array.PushBack( Invalid );
	Array.PushBack( PickLocal );
}





TStateGuiBase::TStateGuiBase(TApp& App) :
	TState	( App )
{
	mCanvas.disableAppEventCallbacks();
	ofAddListener( mCanvas.newGUIEvent, this, &TStateGuiBase::OnCanvasEvent );
}

void TStateGuiBase::Update(float TimeStep)
{
	mCanvas.update();
}

	 
void TStateGuiBase::Render(float TimeStep)
{
	mCanvas.draw();
}


TStatePickMode::TStatePickMode(TApp& App) :
	TStateGui	( App )
{
	auto Rect = *mCanvas.getRect();
	int WidgetHeight = Rect.getHeight() / 10;
	vec2f WidgetSize( WidgetHeight, WidgetHeight );

	CreateEnumButtons( WidgetSize );
}
	


void TStatePickMode::OnButtonClick(TFrontEndWidget::Type Button)
{
	mApp.mInput.CullGestures();

	switch ( Button )
	{
	case TFrontEndWidget::PickLocal:
		mApp.ChangeState<TStateGame>();
		break;
	}
}
