#pragma once

#include "Main.h"

//	included here for cyclic reasons
class TActor;
class TRenderSettings;
class TMaterial;
class TComContainerBase;
template<class TCOM>
class TComContainer;


//---------------------------------
//	change this to a real ref later
//---------------------------------
class TActorRef
{
public:
	TActorRef(const TActor* Actor=NULL) :
		mActor	( Actor )
	{
	}
	TActorRef(const TActor& Actor) :
		mActor	( &Actor )
	{
	}
	
	bool			IsValid() const							{	return mActor != NULL;	}
	TActor*			GetActor()								{	return const_cast<TActor*>( mActor );	}
	const TActor*	GetActor() const						{	return mActor;	}

	operator		bool() const							{	return IsValid();	}
	inline bool		operator==(const TActorRef& Ref) const	{	return mActor == Ref.mActor;	}
	inline bool		operator==(const TActor& Actor) const	{	return mActor == &Actor;	}
	template<typename T>
	inline bool		operator!=(const T& That) const			{	return !((*this)==That);	}

public:
	const TActor*	mActor;
};


//	componenent types
namespace TComs
{
	enum Type
	{
		Invalid,
		Transform,		//	render transform
		Collision,		//	can have collisoin
		Gravity,		//	has attracting-physics
		OwnerPlayer,	//	reference to owner
	};

	void					DestroyComponents(const TActorRef& Ref);
	template<class TCOM>
	TCOM*					GetComponent(const TActorRef& Ref);
	template<class TCOM>
	TComContainer<TCOM>&	GetContainer();

	namespace Private
	{
		extern Array<TComContainerBase*>	gContainerRegistry;
	};
};



//	generic meta information for a componenet
class TComMeta
{
public:
	TComMeta(const TActorRef& Ref) :
		mRef	( Ref )
	{
	}

	TActorRef	GetRef() const							{	return mRef;	}

	inline bool	operator==(const TActorRef& Ref) const	{	return mRef == Ref;	}

public:
	TActorRef	mRef;	//	matches the ref of the owner
};


class TComContainerBase
{
public:
	TComContainerBase();
	virtual ~TComContainerBase();

	virtual void	Destroy(const TActorRef& ComRef)=0;
	bool			Contains(const TActorRef& ComRef)		{	return FindIndex( ComRef ) != -1;	}

protected:
	virtual int		FindIndex(const TActorRef& ComRef)=0;
};

template<class TCOM>
class TComContainer : public TComContainerBase
{
public:
	template<class TCOMMETA>
	TCOM&			Add(const TCOMMETA& ComMeta);
	template<class TCOMMETA,class TDERIVEMETA>
	TCOM&			Add(const TCOMMETA& ComMeta,const TDERIVEMETA& Meta);
	TCOM*			Find(const TActorRef& ComRef)		{	int Index = FindIndex( ComRef );	return (Index < 0) ? NULL : mComponents[Index];	}
	virtual void	Destroy(const TActorRef& ComRef);

protected:
	virtual int		FindIndex(const TActorRef& ComRef);

public:
	Array<TCOM*>	mComponents;
};




template<TComs::Type COMTYPE>
class TCom : public TComMeta
{
protected:
	TCom(const TComMeta& Meta) :
		TComMeta	( Meta )
	{
	}

	//	get a sibling component
	template<class TCOM>
	TCOM*	GetComponent()				{	return TComs::GetContainer<TCOM>().Find( GetRef() );	}	
	template<class TCOM>
	const TCOM*	GetComponent() const	{	return TComs::GetContainer<TCOM>().Find( GetRef() );	}
};



class TComTransform : public TCom<TComs::Transform>, public TTransform2
{
public:
	TComTransform(const TComMeta& ComMeta) :
		TCom				( ComMeta )
	{
	}
	TComTransform(const TComMeta& ComMeta,const TTransform2& Meta) :
		TCom		( ComMeta ),
		TTransform2	( Meta )
	{
	}

	TTransform2&		GetTransform()			{	return *this;	}
	const TTransform2&	GetTransform() const	{	return *this;	}

	void				Render(const TRenderSettings& RenderSettings,const TTransform2& ParentTransform,const TMaterial& Material);
};


class TComOwnerPlayerMeta
{
public:
	TComOwnerPlayerMeta(TRef Player) :
		mPlayer		( Player )
	{
	}

public:
	TRef		mPlayer;
};

class TComOwnerPlayer : public TCom<TComs::OwnerPlayer>, public TComOwnerPlayerMeta
{
public:
	TComOwnerPlayer(const TComMeta& ComMeta,const TComOwnerPlayerMeta& Meta) :
		TCom				( ComMeta ),
		TComOwnerPlayerMeta	( Meta )
	{
	}
};



class TComCollisionMeta
{
public:
	TComCollisionMeta()
	{
	}

public:
	TCollisionShape		mShape;
};

class TComCollision : public TCom<TComs::Collision>, public TComCollisionMeta
{
public:
	TComCollision(const TComMeta& ComMeta) :
		TCom	( ComMeta )
	{
	}

	void		Render(const TRenderSettings& RenderSettings,const TCollisionShape& WorldShape,const TMaterial& Material);
};



class TComGravityMeta
{
public:
	TComGravityMeta()
	{
	}

public:
	ofShapeCircle2		mLocalShape;
};

class TComGravity : public TCom<TComs::Gravity>, public TComGravityMeta
{
public:
	TComGravity(const TComMeta& ComMeta,const TComGravityMeta& Meta=TComGravityMeta()) :
		TCom	( ComMeta )
	{
	}

	void			Render(const TRenderSettings& RenderSettings,const TTransform2& WorldTransform,const TMaterial& Material);
	void			SetLocalGravityShape(const ofShapeCircle2& BoundsShape);
	ofShapeCircle2	GetWorldGravityShape(const TTransform2& WorldTransform);
};









template<class TCOM>
TComContainer<TCOM>& TComs::GetContainer()
{
	static TComContainer<TCOM> gContainer;
	return gContainer;
}

template<class TCOM>
TCOM* TComs::GetComponent(const TActorRef& Ref)
{
	auto& Container = GetContainer<TCOM>();
	return Container.Find( Ref );
}


template<class TCOM>
template<class TCOMMETA>
TCOM& TComContainer<TCOM>::Add(const TCOMMETA& ComMeta)
{
	//	delete existing just in case
	Destroy( ComMeta.GetRef() );

	TCOM* pNewCom = new TCOM( ComMeta );
	mComponents.PushBack( pNewCom );
	return *pNewCom;
}

template<class TCOM>
template<class TCOMMETA,class TDERIVEMETA>
TCOM& TComContainer<TCOM>::Add(const TCOMMETA& ComMeta,const TDERIVEMETA& Meta)
{
	//	delete existing just in case
	Destroy( ComMeta.GetRef() );

	TCOM* pNewCom = new TCOM( ComMeta, Meta );
	mComponents.PushBack( pNewCom );
	return *pNewCom;
}

template<class TCOM>
int TComContainer<TCOM>::FindIndex(const TActorRef& ComRef)
{
	for ( int i=0;	i<mComponents.GetSize();	i++ )
	{
		TCOM* pCom = mComponents[i];
		if ( !pCom )
			continue;
		if ( *pCom == ComRef )
			return i;
	}
	return -1;
}
	
template<class TCOM>
void TComContainer<TCOM>::Destroy(const TActorRef& ComRef)
{
	int Index = FindIndex( ComRef );
	if ( Index < 0 )
		return;

	delete mComponents[Index];
	mComponents[Index] = NULL;
	mComponents.RemoveBlock( Index, 1 );
}

