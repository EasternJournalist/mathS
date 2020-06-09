#pragma once

#include <vector>
#include <string>
#include <functional>

namespace mathS
{
	/*
	The logic of NMathObject:

	NMathObject
	|=>NAtom
	|=>NList
	(The following are not implemented yet)
	|=>//NSparseList
	|=>//NMatrix
	|=>//NSparseMatrix

	NList �� NAtom ���ɹ����ͨ����ļ̳�ʵ�֡�
	��֮��Ե�������ʹ�� NNode���������Ļ����˷�һЩ�ռ䡣

	*/


	// Numeric math library
	namespace NMath
	{
		typedef double NValueType;

		class NMathObject
		{
		public:
			enum Type
			{
				ATOM, LIST, SAPRSE_LIST, ERROR
			};
		public:
			virtual ~NMathObject() {};

			virtual bool IsAtom()const = 0;
			virtual Type GetType() const = 0;
			virtual int Size()const = 0;

			virtual bool IsError() const = 0;
			virtual std::string GetString()const = 0;
		};

		// Atom type of numeric math object.
		class NAtom : public NMathObject
		{
		public:
			NValueType value;
			
			NAtom(NValueType v) :value{ v } {};
			~NAtom() {};

			Type GetType() const { return Type::ATOM; }
			bool IsAtom() const { return true; }
			bool IsError() const { return false; }
			int Size() const { return 1; }

			std::string GetString() const;
		};

		// General list of numeric math object.
		class NList : public NMathObject
		{
		public:
			std::vector<NMathObject*> components;
			
			NList() {};
			NList(std::initializer_list<NMathObject*> _init_list) : components{ _init_list }
			{
			}
			NList(std::initializer_list<NValueType> _init_list)
			{
				for (auto it : _init_list)
					components.push_back(new NAtom(it));
			}
			~NList(){
				for (NMathObject* p : components)
					delete p;
			}

			Type GetType() const { return Type::LIST; }
			bool IsAtom() const { return false; }
			bool IsError() const { return false; }
			int Size() const { return components.size(); };

			NMathObject* PartLocate(const std::vector<int>& loc)const;
			std::string GetString() const;
		};

		// Error type object, which represents an error occurred during calculation.
		class NMathError : public NMathObject
		{
		public:
			std::string info;

			NMathError(const std::string info) : info { info }{};
			~NMathError() {};

			bool IsAtom()const { return false; };
			bool IsError() const { return true; };
			Type GetType() const { return Type::ERROR; };
			int Size()const { return 0; };

			std::string GetString() const{ return info; };
		};
		/*
		class NMatrix : public NMathObject
		{
		public:
			std::vector<NAtom*> components;

			~NMatrix();
		};
		*/


		typedef std::function<NMathObject(NMathObject)> NMathFunction;

		
		//typedef std::function<NValueType(NValueType, NValueType)> NBinaryOperator;
		template<class NBinaryOperator> NMathObject* ShapeWiseTemplate(NMathObject* a, NMathObject* b) 
		{
			static NBinaryOperator op;
			using NType = NMathObject::Type;
			NType atype = a->GetType(), btype = b->GetType();
			// Case 1 (Basic case)
			if (atype == NType::ATOM && btype == NType::ATOM)
			{
				return new NAtom(op(dynamic_cast<NAtom*>(a)->value, dynamic_cast<NAtom*>(b)->value));
			}
			// Case 2
			if (atype == NType::ATOM && btype == NType::LIST)
			{
				NList* ret;
				NList* blist = dynamic_cast<NList*>(b);
				for (auto it : blist->components)
					ret->components.push_back(Plus(a, it));
				return ret;
			}
			// Case 3
			if (atype == NType::LIST && btype == NType::ATOM)
			{
				NList* ret;
				NList* alist = dynamic_cast<NList*>(a);
				for (auto it : alist->components)
					ret->components.push_back(Plus(it, b));
				return ret;
			}
			// Case 4
			if (atype == NType::LIST && btype == NType::LIST)
			{
				NList* alist = dynamic_cast<NList*>(a), * blist = dynamic_cast<NList*>(b);
				int absize;
				if ((absize = alist->Size()) != blist->Size())
					return new NMathError("Shape-wise operation : Shapes of " + alist->GetString() + " and " + blist->GetString() + "do not match. ");

				NList* ret;
				NMathObject* newnode;
				for (int i = 0; i < absize; i++)
				{
					newnode = ShapeWiseTemplate<NBinaryOperator>(alist->components[i], blist->components[i]);
					if (newnode->IsError())
					{
						delete ret;
						// This is very important! When an error occurs, 
						// temporary result mush be deleted before returning an error flag.
						return newnode;
					}
					ret->components.push_back(newnode);
				}
				return ret;
			}
			// Obvious Error Case
			if (atype == NType::ERROR || btype == NType::ERROR)
			{
				return new NMathError
				(
					"An error has existed : "
					+ (atype == NType::ERROR ? dynamic_cast<NMathError*>(a)->info : "")
					+ (btype == NType::ERROR ? dynamic_cast<NMathError*>(b)->info : "")
				);
			}
		}

		// Locate a part of obj and return a reference. Note that this is unsafe because it does not check the indices!
		NMathObject*& PartLocate_ref(NMathObject* obj, const std::vector<int>& loc);

		// Locate a part of obj. Cannot modify the depth of obj, since leaf nodes - NAtom is not changable.
		NMathObject* PartLocate(NMathObject* obj, const std::vector<int>& loc);

// Declare a shape wise NMathFunction with name o f FUNCNAME
#define DECLARE_SHAPE_WISE_NMATHUNC_OP(FUNCNAME) NMathObject* FUNCNAME(NMathObject* const a, NMathObject* const b);

		// + - * /
		DECLARE_SHAPE_WISE_NMATHUNC_OP(Plus)
		DECLARE_SHAPE_WISE_NMATHUNC_OP(Subtract)
		DECLARE_SHAPE_WISE_NMATHUNC_OP(Multiply)
		DECLARE_SHAPE_WISE_NMATHUNC_OP(Divide)

		// Basic NMath Functions
		
#define DECLARE_SHAPE_WISE_NMATHUNC_MONO(FUNCNAME) NMathObject* FUNCNAME(NMathObject* const a);

		DECLARE_SHAPE_WISE_NMATHUNC_MONO(Sin)
		DECLARE_SHAPE_WISE_NMATHUNC_MONO(Cos)
		DECLARE_SHAPE_WISE_NMATHUNC_MONO(Tan)
		DECLARE_SHAPE_WISE_NMATHUNC_MONO(ArcSin)
		DECLARE_SHAPE_WISE_NMATHUNC_MONO(ArcCos)
		DECLARE_SHAPE_WISE_NMATHUNC_MONO(ArcTan)
		DECLARE_SHAPE_WISE_NMATHUNC_MONO(Log)
		DECLARE_SHAPE_WISE_NMATHUNC_MONO(Exp)
		DECLARE_SHAPE_WISE_NMATHUNC_MONO(Floor)

		}
	

	/*
	// Sparse MathObject
	class NSparseList :public NMathObject
	{

	};
	class NSparseMatrix : public NMathObject
	{

	};

	class NValuePlusOp
		{
		public:
			inline NValueType operator()(NValueType _Left, NValueType _Right) const
			{
				return _Left + _Right;
			}
		};
	*/
	

}



