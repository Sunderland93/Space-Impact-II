#include "enemies.h"
#include "graphics.h"
#include "scenery.h"

/** Tájadat struktúra **/
typedef struct SceneryData {
    Graphics FirstObject; /* Az első objektum grafikai azonosítója */
    Uint8 Objects; /* Az objektumok száma, ennyi tartozik a szinthez az előzőleg megadott azonosítótól kezdve */
    Uint8 Upper; /* Fent jelenik meg */
} SceneryData;

/** A szintekhez tartozó tájadatok **/
SceneryData ScData[6] = {
    { 0, 0, 0 }, /* Az 1. szinten nincs táj */
    { 256 + 0, 2, 0 }, /* 2. szint, 0. dinamikus helytől 2 elemű, 700 pixel széles táj */
    { 256 + 2, 6, 0 }, /* 3. szint, 2. dinamikus helytől 6 elemű, 750 pixel széles táj */
    { 256 + 8, 6, 0 }, /* 4. szint, 8. dinamikus helytől 6 elemű, 1000 pixel széles táj */
    { 256 + 14, 4, 1 }, /* 5. szint, 14. dinamikus helytől 4 elemű, 1250 pixel széles felső táj */
    { 256 + 14, 4, 1 }, /* 6. szint, az 5. szint elemeiből, 1600 pixel szélesen */
};

/** Tájelem hozzáadása a paraméterben kapott listához, grafikai azonosítóval, pozícióban **/
void AddScenery(SceneryList *List, Graphics Model, Vec2 Pos) {
    Scenery *CreateAt = *List; /* A létrehozási memóriahely, ahova íródik a lista új utolsó eleme */
    if (*List) { /* Ha van már lista, meg kell keresni a végét, és ott létrehozni az új elemet */
        while (CreateAt->Next) /* Az új cím a listán végiglépked az utolsó elemig */
            CreateAt = CreateAt->Next;
        CreateAt->Next = (Scenery*)malloc(sizeof(Scenery)); /* Az utolsó elem a semmi helyett az újonnan lefoglalt helyre mutat */
        CreateAt = CreateAt->Next; /* Az íráshoz használt cím kerüljön a változóba */
    } else /* Ha nincs még lista, a pointerre kell lefoglalni a címet */
        CreateAt = *List = (Scenery*)malloc(sizeof(Scenery));
    /* Bemenetként kapott adatok átadása az új elemnek */
    CreateAt->Model = Model;
    CreateAt->Pos = Pos;
    CreateAt->Next = NULL; /* Nincs következő elem, lista vége jel */
}

/** A paraméterben kapott táj törlése **/
void EmptyScenery(SceneryList *List) {
    Scenery* Last = *List; /* Utolsó elem címének tárolása a törléshez */
    while (*List) { /* Ameddig van még elem */
        *List = (*List)->Next; /* A pointer lépjen a következő elemre */
        free(Last); /* Az előzőleg vizsgált elem felszabadítása */
        Last = *List; /* Most már ez az utoljára vizsgált elem */
    }
    *List = NULL; /* Ne mutasson sehova, mert nincs semmi lefoglalva */
}

void HandleScenery(SceneryList *List, Uint8 *PixelMap, Uint8 Move, PlayerObject *Player, Sint8 Level) {
    Scenery *First = *List;
    Sint16 LastX = 0;
    Object Model;

    while (*List) {
        if (Move)
            (*List)->Pos.x--;

        Model = GetObject((*List)->Model);

        if (Level != 1 && Intersect((*List)->Pos, Model.Size, Player->Pos, NewVec2(10, 7)))
            Player->Lives--;

        if ((*List)->Pos.x < -Model.Size.x) {
            *List = (*List)->Next;
            free(First);
            First = *List;
        } else {
            LastX = (*List)->Pos.x + Model.Size.x;
            DrawObject(PixelMap, Model, (*List)->Pos);
            *List = (*List)->Next;
        }
    }

    if (Level != 0 && ScData[Level].Objects > 0) {
        while (LastX < 84) {
            Object Model;
            Scenery* NewScenery = (Scenery*)malloc(sizeof(Scenery));
            NewScenery->Model = ScData[Level].FirstObject + rand() % ScData[Level].Objects;
            Model = GetObject(NewScenery->Model);
            NewScenery->Pos = NewVec2(LastX, ScData[Level].Upper ? 0 : 48 - Model.Size.y);
            NewScenery->Next = NULL;
            LastX = NewScenery->Pos.x + Model.Size.x;

            if (First == NULL)
                First = NewScenery;
            else {
                for (*List = First; (*List)->Next; *List = (*List)->Next);
                (*List)->Next = NewScenery;
            }
        }
    }

    *List = First;
}
