#pragma once
#include <array> 
	template <typename Type, int size, typename	CountType = unsigned short, bool UseSafeModeFlag = true>
	class VectorOnStack
	{
	public:
		VectorOnStack();

		VectorOnStack(const VectorOnStack& aVectorOnStack)
		{
			/*
			Klassen m�ste supporta en copyconstruktor.Denna copy konstruktorn m�ste ta
			en fysisk kopia av all data i andra instansen av klassen och l�gga in i den f�rsta.
			VG: Om safemode flaggan �r false s� ska all kopiering av datan g�ras med
			memcpy.
			*/
		}

		~VectorOnStack()
		{
			/*Klassens destruktor ska �terst�lla alla varibler i klassen till 0 (dock inte sj�lva
				datan)*/
		}

		VectorOnStack& operator=(const VectorOnStack&
			aVectorOnStack)
		{/*
			Klassen m�ste supporta en tilldelningsoperator.Denna operatorn m�ste ta en
				fysisk kopia av all data i andra instancens av klassen och l�gga in i den f�rsta.
				VG: Om safemode flaggan �r false s� ska all kopiering av datan g�ras med
				memcpy.*/
		}

		inline const Type& operator[](const CountType aIndex) const
		{/*
			Klassen ska supporta operator[].Denna operatorn ska till�ta folk att l�sa av och
				tilldela element av arrayen som redan f�tt data.Om elementen ligger utanf�r
				nuvarande range, dvs.mindre �n 0 eller st�rre �n nuvarande m�ngd element i
				arrayen s� ska den asserta.Annars ska den returnera en referens till datan vid
				positionen.
				Av performancesk�l ska funktionen vara inline.
				Funktionen ska fungera p� const instanser av klassen.*/
		}
		 

		inline void Add(const Type& aObject)
		{
			/*
			Funktionen Add ska ta och l�gga till elementet som skickas in till den sista lediga
				platsen i den interna arrayen.Detta m�ste ske i konstant tid.Om arrayn p�
				stacken �r full s� ska klassen asserta.Klassen h�ller internt sj�lv reda p� vilken
				som �r n�sta lediga plats i listan.
				VG: Om safemode flaggan �r false s� ska datan l�gga till med memcpy.
				Av performancesk�l ska funktionen vara inline.*/
		}

		inline void Insert(const CountType aIndex, const Type& aObject)
		{
			/*Insert ska g�ra vad namnet s�ger den ska s�tta in ett objekt p� den valda
				positionen i listan medans den bibeh�ller den relativa ordningen p� alla andra
				element i listan.Om listan redan �r full s� ska programmet asserta.
				Det ska g� att inserta p� 0 �ven om containern �r tom.N�r det finns objekt i den
				ska det g�r att inserta efter sista elementet.
				Av performancesk�l ska funktionen vara inline.
				VG: Om safemode �r false ska memcpy anv�ndas f�r att flytta datan*/
		}

		inline void RemoveCyclic(const Type& aObject)
		{/*
			Funktionen ska s�ka igenom arrayen efter det efterfr�gade objekten sen ta bort
				det genom att kopiera in det sista elementet �ver det borttagna elementets plats.
				Av performancesk�l ska funktionen vara inline.*/
		}

		inline void RemoveCyclicAtIndex(const CountType aIndex)
		{/*
			Funktionen ska ta bort elementet p� den angivna positionen genom att kopiera
				in det sista elementet �ver det borttagna elementets plats.
				Av performancesk�l ska funktionen vara inline.*/
		}

		inline void Clear()
		{/*
			Funktionen ska g�ra s� att klassen agerar som om listan vore tom.
				Av performancesk�l ska funktionen vara inline.*/
		}

		__forceinline CountType Size() const
		{/*
			Funktionen ska returnera m�ngden objekt i klassen just nu.
				Funktionen ska fungera p� const instanser av klassen
				Av performancesk�l ska funktionen vara forceinline.*/
		}

	private:
		int count;
		std::array<Type,size> array;

	};



	// H�r p�b�rjar deklarationen f�r varje funktion
	// Exempel f�r Default Konstruktorn:
	template <typename Type, int size, typename CountType, bool UseSafeModeFlag>
	VectorOnStack<Type, size, CountType, UseSafeModeFlag>::VectorOnStack()
	{
		// Konstruktor kod, klassen ska vara i fullt skick att
		// anv�ndas efter konstruktorn har k�rts.
	} 