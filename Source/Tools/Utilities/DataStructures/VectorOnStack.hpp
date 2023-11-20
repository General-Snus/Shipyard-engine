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
			Klassen måste supporta en copyconstruktor.Denna copy konstruktorn måste ta
			en fysisk kopia av all data i andra instansen av klassen och lägga in i den första.
			VG: Om safemode flaggan är false så ska all kopiering av datan göras med
			memcpy.
			*/
		}

		~VectorOnStack()
		{
			/*Klassens destruktor ska återställa alla varibler i klassen till 0 (dock inte själva
				datan)*/
		}

		VectorOnStack& operator=(const VectorOnStack&
			aVectorOnStack)
		{/*
			Klassen måste supporta en tilldelningsoperator.Denna operatorn måste ta en
				fysisk kopia av all data i andra instancens av klassen och lägga in i den första.
				VG: Om safemode flaggan är false så ska all kopiering av datan göras med
				memcpy.*/
		}

		inline const Type& operator[](const CountType aIndex) const
		{/*
			Klassen ska supporta operator[].Denna operatorn ska tillåta folk att läsa av och
				tilldela element av arrayen som redan fått data.Om elementen ligger utanför
				nuvarande range, dvs.mindre än 0 eller större än nuvarande mängd element i
				arrayen så ska den asserta.Annars ska den returnera en referens till datan vid
				positionen.
				Av performanceskäl ska funktionen vara inline.
				Funktionen ska fungera på const instanser av klassen.*/
		}
		 

		inline void Add(const Type& aObject)
		{
			/*
			Funktionen Add ska ta och lägga till elementet som skickas in till den sista lediga
				platsen i den interna arrayen.Detta måste ske i konstant tid.Om arrayn på
				stacken är full så ska klassen asserta.Klassen håller internt själv reda på vilken
				som är nästa lediga plats i listan.
				VG: Om safemode flaggan är false så ska datan lägga till med memcpy.
				Av performanceskäl ska funktionen vara inline.*/
		}

		inline void Insert(const CountType aIndex, const Type& aObject)
		{
			/*Insert ska göra vad namnet säger den ska sätta in ett objekt på den valda
				positionen i listan medans den bibehåller den relativa ordningen på alla andra
				element i listan.Om listan redan är full så ska programmet asserta.
				Det ska gå att inserta på 0 även om containern är tom.När det finns objekt i den
				ska det går att inserta efter sista elementet.
				Av performanceskäl ska funktionen vara inline.
				VG: Om safemode är false ska memcpy användas för att flytta datan*/
		}

		inline void RemoveCyclic(const Type& aObject)
		{/*
			Funktionen ska söka igenom arrayen efter det efterfrågade objekten sen ta bort
				det genom att kopiera in det sista elementet över det borttagna elementets plats.
				Av performanceskäl ska funktionen vara inline.*/
		}

		inline void RemoveCyclicAtIndex(const CountType aIndex)
		{/*
			Funktionen ska ta bort elementet på den angivna positionen genom att kopiera
				in det sista elementet över det borttagna elementets plats.
				Av performanceskäl ska funktionen vara inline.*/
		}

		inline void Clear()
		{/*
			Funktionen ska göra så att klassen agerar som om listan vore tom.
				Av performanceskäl ska funktionen vara inline.*/
		}

		__forceinline CountType Size() const
		{/*
			Funktionen ska returnera mängden objekt i klassen just nu.
				Funktionen ska fungera på const instanser av klassen
				Av performanceskäl ska funktionen vara forceinline.*/
		}

	private:
		int count;
		std::array<Type,size> array;

	};



	// Här påbörjar deklarationen för varje funktion
	// Exempel för Default Konstruktorn:
	template <typename Type, int size, typename CountType, bool UseSafeModeFlag>
	VectorOnStack<Type, size, CountType, UseSafeModeFlag>::VectorOnStack()
	{
		// Konstruktor kod, klassen ska vara i fullt skick att
		// användas efter konstruktorn har körts.
	} 