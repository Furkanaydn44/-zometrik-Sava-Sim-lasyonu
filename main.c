//230202071_230202101

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#include <curl/curl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define MAX_INFANTRY_LIMIT 100
#define MAX_LINE_LENGTH 256
#include "renderer.h"
#include "initclose.h"
#include "texture.h"
#include "isoEngine.h"

extern int TILESIZE;
#define NUM_ISOMETRIC_TILES 5
#define MAP_HEIGHT 20
#define MAP_WIDTH 20

///////////////////////////////////////////////////////////////////research struct///////////////////////////////////////////////////////////////////////


    struct research
    {
        char isim[30];
        int seviye;
        int oran;
        char aciklama[50];
    };

///////////////////////////////////////////////////////////////////army nums//////////////////////////////////////////////////////

struct human_imperium_army
{
    int infantryn;
    int archern;
    int cavalryn;
    int siege_machinen;
    //kahramanlar
    short alparslan;
    short fatih_sm;
    short mete_han;
    short yavuz_ss;
    short tugrul_bey;
    //canavarlar
    short dragon;
    short agri_mountain_giants;
    short goliath;
    short blackwolf;
    short samur;
    //arastirma
    char h_arastirma[30];
    int h_arastirmalvl;
};

struct ork_legi_army
{
    int ork_fightern;
    int ork_spearern;
    int varg_ridern;
    int trolln;
    //kahramanlar
    short goruk;
    short thruk_bonecrusher;
    short vrog_headsmasher;
    short ugar;
    //canavarlar
    short dark_troll;
    short shadow_wolfs;
    short mud_giants;
    short fire_demon;
    short makrog;
    short ice_giants;
    //arastirma
    char o_arastirma[30];
    int o_arastirmalvl;
};

////////////////////////////////////////////////////////////infantry struct//////////////////////////////////////////////////////////



struct infantry
{
    float saldiri;
    float savunma;
    int saglik;
    float kritik_sans;

};


///////////////////////////////////////////////////////////////hero struct////////////////////////////////////////////////////////////




struct hero
{
    char bonus_turu[15];
    int bonus_degeri;
    char aciklama[25];

};



///////////////////////////////////////////////////////////////creature struct/////////////////////////////////////////////////////////


struct creature
{
    int etki_degeri;
    char etki_turu[15];
    char aciklama[25];
};




///////////////////////////////////////////////////////////parent structlar/////////////////////////////////////////////////////////////

    struct unitlist{
    struct infantry units[8];
    /*  [0] piyadeler
        [1] suvariler
        [2] okcular
        [3] kusatma_makineleri
        [4] ork_dovusculeri
        [5] mizrakcilar
        [6] varg_binicileri
        [7] troller
    */
    };
    struct creaturelist{
    struct creature cs[11];
    /*
        [0] Ejderha
        [0] Agri_Dagi_Devleri
        [0] Tepegoz
        [0] Karakurt
        [0] Samur
        [0] Kara_Troll
        [0] Golge_Kurtlari
        [0] Camur_Devleri
        [0] Ates_Iblisi
        [0] Makrog_Savas_Beyi
        [0] Buz_Devleri
    */
    };
    struct herolist{
    struct hero hs[9];
/*
        [0] Alparslan
        [1] Fatih_Sultan_Mehmet
        [2] Mete_Han
        [3] Yavuz_Sultan_Selim
        [4] Tugrul_Bey
        [5] Goruk_Vahsi
        [6] Thruk_Kemikkiran
        [7] Vrog_Kafakiran
        [8] Ugar_Zalim
*/
    };
    struct researchlist{
    struct research ss[12];
/*
        [0] savunma_ustaligi
        [1] savunma_ustaligi
        [2] savunma_ustaligi
        [3] saldiri_gelistirmesi
        [4] saldiri_gelistirmesi
        [5] saldiri_gelistirmesi
        [6] elit_egitim
        [7] elit_egitim
        [8] elit_egitim
        [9] kusatma_ustaligi
        [10] kusatma_ustaligi
        [11] kusatma_ustaligi
*/
    };


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *downloaded)
{
    size_t written = fwrite(ptr, size, nmemb, downloaded);
    return written;
}

void curl_func()
{
    FILE *downloaded;
    CURL *curl;
    CURLcode error;
    int choice;

    const char *url[10] = {"https://yapbenzet.org.tr/1.json","https://yapbenzet.org.tr/2.json","https://yapbenzet.org.tr/3.json","https://yapbenzet.org.tr/4.json"
    ,"https://yapbenzet.org.tr/5.json","https://yapbenzet.org.tr/6.json","https://yapbenzet.org.tr/7.json"
    ,"https://yapbenzet.org.tr/8.json","https://yapbenzet.org.tr/9.json","https://yapbenzet.org.tr/10.json"};
    const char *outfilename = "senerio.json";
    for(int i=0;i<10;i++)
        printf("%d.url:%s\n",i+1,url[i]);
    do{
    printf("Secmek istediginiz seneryonun url numarasini giriniz:");
    scanf("%d",&choice);
    curl = curl_easy_init();
    if (curl) {

        downloaded = fopen(outfilename, "wb");
        if (downloaded == NULL) {
            printf("Dosya acilamadi");
        }
        curl_easy_setopt(curl, CURLOPT_URL, url[(choice-1)]);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, downloaded);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        error = curl_easy_perform(curl);

        if (error == CURLE_OK) {
            printf("Dosya basariyla indirildi\n");

        } else {
            printf("Dosya indirilemedi\nTekrar Deneyiniz.\n");
        }}}while(error!=CURLE_OK);

        fclose(downloaded);
        curl_easy_cleanup(curl);

}
void senerio_unit_parser(const char *keyword, struct human_imperium_army *hseneryo, struct ork_legi_army *oseneryo)
{
    FILE *dosya = fopen("senerio.json","r");
    if(dosya == NULL)
    {
        printf("DOSYA ACILMADI\n");
        return;
    }
    char satir[MAX_LINE_LENGTH];

        while(fgets(satir,sizeof(satir),dosya))
        {
            char *kelime = strstr(satir,keyword);
            if(kelime != NULL)
            {
                kelime += strlen(keyword);
                while (*kelime && (*kelime == ' ' || *kelime == ':' || *kelime == '\t' || *kelime == '"'))
                {
                    kelime++;
                }
                int sayac;
                if(sscanf(kelime,"%d",&sayac) == 1)
                {
                    if(strcmp(keyword,"piyadeler")==0)
                        hseneryo->infantryn=sayac;
                    if(strcmp(keyword,"okcular")==0)
                        hseneryo->archern=sayac;
                    if(strcmp(keyword,"suvariler")==0)
                        hseneryo->cavalryn=sayac;
                    if(strcmp(keyword,"kusatma_makineleri")==0)
                        hseneryo->siege_machinen=sayac;
                    if(strcmp(keyword,"ork_dovusculeri")==0)
                        oseneryo->ork_fightern=sayac;
                    if(strcmp(keyword,"mizrakcilar")==0)
                        oseneryo->ork_spearern=sayac;
                    if(strcmp(keyword,"varg_binicileri")==0)
                        oseneryo->varg_ridern=sayac;
                    if(strcmp(keyword,"troller")==0)
                        oseneryo->trolln=sayac;
                }
            }
        }
    fclose(dosya);
}

void senerio_hero_parser(const char *keyword, struct human_imperium_army *hseneryo, struct ork_legi_army *oseneryo)
{
    FILE *dosya = fopen("senerio.json","r");
    if(dosya == NULL)
    {
        printf("Dosya %s acilamadi\n","senerio.json");
        return;
    }
    char satir[MAX_LINE_LENGTH];
        while(fgets(satir,sizeof(satir),dosya))
        {
            char *kelime = strstr(satir,keyword);
            if(kelime != NULL)
            {
        if(strcmp(keyword,"Ejderha")==0)
        hseneryo->dragon=1;
        if(strcmp(keyword,"Agri_Dagi_Devleri")==0)
        hseneryo->agri_mountain_giants=1;
        if(strcmp(keyword,"Tepegoz")==0)
        hseneryo->goliath=1;
        if(strcmp(keyword,"Karakurt")==0)
        hseneryo->blackwolf=1;
        if(strcmp(keyword,"Samur")==0)
        hseneryo->samur=1;
        if(strcmp(keyword,"Kara_Troll")==0)
        oseneryo->dark_troll=1;
        if(strcmp(keyword,"Golge_Kurtlari")==0)
        oseneryo->shadow_wolfs=1;
        if(strcmp(keyword,"Camur_Devleri")==0)
        oseneryo->mud_giants=1;
        if(strcmp(keyword,"Ates_Iblisi")==0)
        oseneryo->fire_demon=1;
        if(strcmp(keyword,"Makrog_Savas_Beyi")==0)
        oseneryo->makrog=1;
        if(strcmp(keyword,"Alparslan")==0)
        hseneryo->alparslan=1;
        if(strcmp(keyword,"Fatih_Sultan_Mehmet")==0)
        hseneryo->fatih_sm=1;
        if(strcmp(keyword,"Mete_Han")==0)
        hseneryo->mete_han=1;
        if(strcmp(keyword,"Yavuz_Sultan_Selim")==0)
        hseneryo->yavuz_ss=1;
        if(strcmp(keyword,"Tugrul_Bey")==0)
        hseneryo->tugrul_bey=1;
        if(strcmp(keyword,"Goruk_Vahsi")==0)
        oseneryo->goruk=1;
        if(strcmp(keyword,"Thruk_Kemikkiran")==0)
        oseneryo->thruk_bonecrusher=1;
        if(strcmp(keyword,"Vrog_Kafakiran")==0)
        oseneryo->vrog_headsmasher=1;
        if(strcmp(keyword,"Ugar_Zalim")==0)
        oseneryo->ugar=1;
            }
        }
    fclose(dosya);
}

void senerio_creature_parser(const char *keyword,struct human_imperium_army *hseneryo, struct ork_legi_army *oseneryo)
{
    FILE *dosya = fopen("senerio.json","r");
    if(dosya == NULL)
    {
         printf("Dosya %s acilamadi\n","senerio.json");
        return;
    }
    char satir[MAX_LINE_LENGTH];

        while(fgets(satir,sizeof(satir),dosya))
        {
            char *kelime = strstr(satir,keyword);
            if(kelime != NULL)
            {
        if(strcmp(keyword,"Ejderha")==0)
        hseneryo->dragon=1;
        if(strcmp(keyword,"Agri_Dagi_Devleri")==0)
        hseneryo->agri_mountain_giants=1;
        if(strcmp(keyword,"Tepegoz")==0)
        hseneryo->goliath=1;
        if(strcmp(keyword,"Karakurt")==0)
        hseneryo->blackwolf=1;
        if(strcmp(keyword,"Samur")==0)
        hseneryo->samur=1;
        if(strcmp(keyword,"Kara_Troll")==0)
        oseneryo->dark_troll=1;
        if(strcmp(keyword,"Golge_Kurtlari")==0)
        oseneryo->shadow_wolfs=1;
        if(strcmp(keyword,"Camur_Devleri")==0)
        oseneryo->mud_giants=1;
        if(strcmp(keyword,"Ates_Iblisi")==0)
        oseneryo->fire_demon=1;
        if(strcmp(keyword,"Makrog_Savas_Beyi")==0)
        oseneryo->makrog=1;
        if(strcmp(keyword,"Buz_Devleri")==0)
        oseneryo->ice_giants=1;
        if(strcmp(keyword,"Alparslan")==0)
        hseneryo->alparslan=1;
        if(strcmp(keyword,"Fatih_Sultan_Mehmet")==0)
        hseneryo->fatih_sm=1;
        if(strcmp(keyword,"Mete_Han")==0)
        hseneryo->mete_han=1;
        if(strcmp(keyword,"Yavuz_Sultan_Selim")==0)
        hseneryo->yavuz_ss=1;
        if(strcmp(keyword,"Tugrul_Bey")==0)
        hseneryo->tugrul_bey=1;
        if(strcmp(keyword,"Goruk_Vahsi")==0)
        oseneryo->goruk=1;
        if(strcmp(keyword,"Thruk_Kemikkiran")==0)
        oseneryo->thruk_bonecrusher=1;
        if(strcmp(keyword,"Vrog_Kafakiran")==0)
        oseneryo->vrog_headsmasher=1;
        if(strcmp(keyword,"Ugar_Zalim")==0)
        oseneryo->ugar=1;
            }
        }
    fclose(dosya);
}

void senerio_research_parser(const char *aranankelime,struct human_imperium_army *hseneryo, struct ork_legi_army *oseneryo)
{
FILE *dosya = fopen("senerio.json","r");
    if(dosya == NULL)
    {
         printf("Dosya %s acilamadi\n","senerio.json");
        return;
    }
    char satir[MAX_LINE_LENGTH];
    int insan_imparatorlugu=0;
    int ork_legi=0;
      while(fgets(satir,sizeof(satir),dosya))
      {
          if(strstr(satir, "insan_imparatorlugu") != NULL){
            insan_imparatorlugu = 1;
            ork_legi = 0;
          }
          if(strstr(satir,"ork_legi") != NULL){
            insan_imparatorlugu = 0;
            ork_legi = 1;
          }
          char *kelime = strstr(satir,aranankelime);
          if(kelime != NULL){
             kelime += strlen(aranankelime);
                while (*kelime && (*kelime == ' ' || *kelime == ':' || *kelime == '\t' || *kelime == '"'))
                {
                    kelime++;
                }
                int sayac;
                if(sscanf(kelime,"%d",&sayac) == 1){
                if(insan_imparatorlugu){
                strcpy(hseneryo->h_arastirma,aranankelime);
                hseneryo->h_arastirmalvl=sayac;
                                       }
               else if(ork_legi){
                strcpy(oseneryo->o_arastirma,aranankelime);
                oseneryo->o_arastirmalvl=sayac;
                                }
                                                }
                            }
          }
      fclose(dosya);
}
void temizle(char *str) {
    char *ptr = str;
    while (*ptr) {

        if (*ptr == ',' || *ptr == ';' || *ptr == '"' || *ptr == '\n') {
            *ptr = '\0';
            break;
        }
        ptr++;
}}
void creature_parser(const char *filename,const char *heroname,const char *arananozellik,struct creaturelist *gamec)
{
    int hnum;
        if(strcmp(heroname,"Ejderha")==0)
        hnum=0;
        if(strcmp(heroname,"Agri_Dagi_Devleri")==0)
        hnum=1;
        if(strcmp(heroname,"Tepegoz")==0)
        hnum=2;
        if(strcmp(heroname,"Karakurt")==0)
        hnum=3;
        if(strcmp(heroname,"Samur")==0)
        hnum=4;
        if(strcmp(heroname,"Kara_Troll")==0)
        hnum=5;
        if(strcmp(heroname,"Golge_Kurtlari")==0)
        hnum=6;
        if(strcmp(heroname,"Camur_Devleri")==0)
        hnum=7;
        if(strcmp(heroname,"Ates_Iblisi")==0)
        hnum=8;
        if(strcmp(heroname,"Makrog_Savas_Beyi")==0)
        hnum=9;
        if(strcmp(heroname,"Buz_Devleri")==0)
        hnum=10;

    FILE *dosya = fopen(filename,"r");
    if(dosya == NULL)
    {
         printf("Dosya %s acilamadi\n",filename);
        return;
    }
    char *buffer=malloc(sizeof(char)*20);
    char satir[MAX_LINE_LENGTH];
    int kahraman_bulundu = 0;
    while(fgets(satir, sizeof(satir), dosya)) {

        char *kelime1 = strstr(satir, heroname);
        if (kelime1 != NULL) {
            kahraman_bulundu = 1;
        }
        if (kahraman_bulundu) {

            char *kelime2 = strstr(satir, arananozellik);
            if (kelime2 != NULL) {
                kelime2 += strlen(arananozellik);
                while (*kelime2 && (*kelime2 == ' ' || *kelime2 == ':' || *kelime2 == '\t' || *kelime2 == '"' || *kelime2 == '{' || *kelime2 == ',')) {
                    kelime2++;
                }
                temizle(kelime2);
                if (strcmp(arananozellik, "etki_degeri") == 0) {
                    int sayac;
                    if (sscanf(kelime2, "%d", &sayac) == 1) {
                        gamec->cs[hnum].etki_degeri=sayac;
                    }
                }
                else if(strcmp(arananozellik, "aciklama") == 0){
                    char kelime3[20];
                    if(sscanf(kelime2,"%s",kelime3) == 1) {
                       if(strcmp(kelime3,"piyadelere")==0||strcmp(kelime3,"piyadelerin")==0)
                    {   strcpy(buffer,"piyade");
                        strcpy(gamec->cs[hnum].aciklama,"asdad");}
                        if(strcmp(kelime3,"okculara")==0||strcmp(kelime3,"okcularin")==0)
                    {   strcpy(buffer,"okcu");
                        strcpy(gamec->cs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"suvarilere")==0||strcmp(kelime3,"suvarilerin")==0)
                    {   strcpy(buffer,"suvari");
                        strcpy(gamec->cs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"kusatma_makinelerine")==0||strcmp(kelime3,"kusatma_makinelerinin")==0)
                    {   strcpy(buffer,"kusatma_makine");
                        strcpy(gamec->cs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"ork_dovusculerine")==0||strcmp(kelime3,"ork_dovusculerinin")==0)
                    {   strcpy(buffer,"ork_dovuscu");
                        strcpy(gamec->cs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"mizrakcilara")==0||strcmp(kelime3,"mizrakcilarin")==0)
                    {   strcpy(buffer,"mizrakci");
                        strcpy(gamec->cs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"varg_binicilerine")==0||strcmp(kelime3,"varg_binicilerinin")==0||strcmp(kelime3,"varg_binicilerin")==0)
                    {   strcpy(buffer,"varg_binici");
                        strcpy(gamec->cs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"trollere")==0||strcmp(kelime3,"trollerin")==0)
                    {   strcpy(buffer,"troll");
                        strcpy(gamec->cs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"tum_birimlere")==0||strcmp(kelime3,"tum_birimlerin")==0)
                    {   strcpy(buffer,"tum_birim");
                        strcpy(gamec->cs[hnum].aciklama,buffer);}
                    }
                }
                else {
                    strcpy(gamec->cs[hnum].etki_turu,kelime2);
                }
                kahraman_bulundu = 0;
            }
        }
    }
    fclose(dosya);
}
void hero_parser(const char *filename,const char *heroname,const char *arananozellik,struct herolist *gameh)
{
    int hnum;
        if(strcmp(heroname,"Alparslan")==0)
        hnum=0;
        if(strcmp(heroname,"Fatih_Sultan_Mehmet")==0)
        hnum=1;
        if(strcmp(heroname,"Mete_Han")==0)
        hnum=2;
        if(strcmp(heroname,"Yavuz_Sultan_Selim")==0)
        hnum=3;
        if(strcmp(heroname,"Tugrul_Bey")==0)
        hnum=4;
        if(strcmp(heroname,"Goruk_Vahsi")==0)
        hnum=5;
        if(strcmp(heroname,"Thruk_Kemikkiran")==0)
        hnum=6;
        if(strcmp(heroname,"Vrog_Kafakiran")==0)
        hnum=7;
        if(strcmp(heroname,"Ugar_Zalim")==0)
        hnum=8;

    FILE *dosya = fopen(filename,"r");
    if(dosya == NULL)
    {
            printf("Dosya %s acilamadi\n",filename);
        return;
    }
    char *buffer=malloc(sizeof(char)*20);
    char satir[MAX_LINE_LENGTH];
    int kahraman_bulundu = 0;
    while(fgets(satir, sizeof(satir), dosya)) {

        char *kelime1 = strstr(satir, heroname);
        if (kelime1 != NULL) {
            kahraman_bulundu = 1;
        }
        if (kahraman_bulundu) {

            char *kelime2 = strstr(satir, arananozellik);
            if (kelime2 != NULL) {
                kelime2 += strlen(arananozellik);
                while (*kelime2 && (*kelime2 == ' ' || *kelime2 == ':' || *kelime2 == '\t' || *kelime2 == '"' || *kelime2 == '{' || *kelime2 == ',')) {
                    kelime2++;
                }
                temizle(kelime2);
                if (strcmp(arananozellik, "bonus_degeri") == 0) {
                    int sayac;
                    if (sscanf(kelime2, "%d", &sayac) == 1) {
                        gameh->hs[hnum].bonus_degeri=sayac;
                    }
                }
                else if(strcmp(arananozellik, "aciklama") == 0){
                    char kelime3[20];
                    if(sscanf(kelime2,"%s",kelime3) == 1) {
                       if(strcmp(kelime3,"piyadelere")==0||strcmp(kelime3,"piyadelerin")==0)
                    {   strcpy(buffer,"piyade");
                        strcpy(gameh->hs[hnum].aciklama,"asdad");}
                        if(strcmp(kelime3,"okculara")==0||strcmp(kelime3,"okcularin")==0)
                    {   strcpy(buffer,"okcu");
                        strcpy(gameh->hs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"suvarilere")==0||strcmp(kelime3,"suvarilerin")==0)
                    {   strcpy(buffer,"suvari");
                        strcpy(gameh->hs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"kusatma_makinelerine")==0||strcmp(kelime3,"kusatma_makinelerinin")==0)
                    {   strcpy(buffer,"kusatma_makine");
                        strcpy(gameh->hs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"ork_dovusculerine")==0||strcmp(kelime3,"ork_dovusculerinin")==0)
                    {   strcpy(buffer,"ork_dovuscu");
                        strcpy(gameh->hs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"mizrakcilara")==0||strcmp(kelime3,"mizrakcilarin")==0)
                    {   strcpy(buffer,"mizrakci");
                        strcpy(gameh->hs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"varg_binicilerine")==0||strcmp(kelime3,"varg_binicilerinin")==0)
                    {   strcpy(buffer,"varg_binici");
                        strcpy(gameh->hs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"trollere")==0||strcmp(kelime3,"trollerin")==0)
                    {   strcpy(buffer,"troll");
                        strcpy(gameh->hs[hnum].aciklama,buffer);}
                        if(strcmp(kelime3,"tum_birimlere")==0||strcmp(kelime3,"tum_birimlerin")==0)
                    {   strcpy(buffer,"tum_birim");
                        strcpy(gameh->hs[hnum].aciklama,buffer);}
                    }
                }
                else {
                    strcpy(gameh->hs[hnum].bonus_turu,kelime2);
                }
                kahraman_bulundu = 0;
            }
        }
    }
    fclose(dosya);
}
void unit_parser(struct unitlist *gamei,const char *filename,const char *aranankelime,const char *arananozellik)
{
    FILE *dosya = fopen(filename,"r");
    if(dosya == NULL)
    {
             printf("Dosya %s acilamadi\n",filename);
        return;
    }
    char satir[MAX_LINE_LENGTH];
    while(fgets(satir,sizeof(satir),dosya))
    {
        char *kelime1 = strstr(satir,aranankelime);
        if(kelime1 != NULL){
            for(int i = 0;fgets(satir, sizeof(satir), dosya) &&  i < 4 ;i++){
                char *kelime2 = strstr(satir,arananozellik);
                if(kelime2 != NULL){
                    kelime2 += strlen(arananozellik);
                while (*kelime2 && (*kelime2 == ' ' || *kelime2 == ':' || *kelime2 == '\t' || *kelime2 == '"'))
                {
                    kelime2++;
                }
                int sayac;
                if(sscanf(kelime2,"%d",&sayac) == 1){

        int unitnum;
        if(strcmp(aranankelime,"piyadeler")==0)
        unitnum=0;
        if(strcmp(aranankelime,"okcular")==0)
        unitnum=1;
        if(strcmp(aranankelime,"suvariler")==0)
        unitnum=2;
        if(strcmp(aranankelime,"kusatma_makineleri")==0)
        unitnum=3;
        if(strcmp(aranankelime,"ork_dovusculeri")==0)
        unitnum=4;
        if(strcmp(aranankelime,"mizrakcilar")==0)
        unitnum=5;
        if(strcmp(aranankelime,"varg_binicileri")==0)
        unitnum=6;
        if(strcmp(aranankelime,"troller")==0)
        unitnum=7;

        if(strcmp(arananozellik,"saldiri")==0)
        gamei->units[unitnum].saldiri=sayac;
        if(strcmp(arananozellik,"savunma")==0)
        gamei->units[unitnum].savunma=sayac;
        if(strcmp(arananozellik,"saglik")==0)
        gamei->units[unitnum].saglik=sayac;
        if(strcmp(arananozellik,"kritik_sans")==0)
        gamei->units[unitnum].kritik_sans=sayac;
                        }
                }}
            }
        }
    }
void research_parser(struct researchlist *gamer, const char *filename, const char *researchname)
{
    FILE *dosya = fopen(filename, "r");
    if (dosya == NULL)
    {
        printf("Dosya %s acilamadi\n",filename);
        return;
    }

    char satir[MAX_LINE_LENGTH];
    int seviye_count = 0;

    while (fgets(satir, sizeof(satir), dosya))
    {
        char *kelime1 = strstr(satir, researchname);
        if (kelime1 != NULL)
        {
            while (seviye_count < 3 && fgets(satir, sizeof(satir), dosya))
            {
                char *seviye_kelime = strstr(satir, "seviye_");
                if (seviye_kelime != NULL)
                {
                    int mevcut_seviye;
                    if (sscanf(seviye_kelime, "seviye_%d", &mevcut_seviye) == 1 && mevcut_seviye > 0 && mevcut_seviye <= 3)
                    {
                        mevcut_seviye--;

                        while (fgets(satir, sizeof(satir), dosya))
                        {
                            char *kelime2 = strstr(satir, "deger");
                            if (kelime2 != NULL)
                            {
                                kelime2 += strlen("deger");
                                while (*kelime2 && (*kelime2 == ' ' || *kelime2 == ':' || *kelime2 == '\t' || *kelime2 == '"'))
                                {
                                    kelime2++;
                                }

                                int oran;
                                if (sscanf(kelime2, "%d", &oran) == 1)
                                {
                                    int index = mevcut_seviye;
                                    if (strcmp(researchname, "savunma_ustaligi") == 0)
                                        index = mevcut_seviye;
                                    else if (strcmp(researchname, "saldiri_gelistirmesi") == 0)
                                        index = 3 + mevcut_seviye;
                                    else if (strcmp(researchname, "elit_egitim") == 0)
                                        index = 6 + mevcut_seviye;
                                    else if (strcmp(researchname, "kusatma_ustaligi") == 0)
                                        index = 9 + mevcut_seviye;

                                    strcpy(gamer->ss[index].isim, researchname);
                                    gamer->ss[index].oran = oran;
                                    gamer->ss[index].seviye = mevcut_seviye+1;

                                    seviye_count++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (seviye_count >= 3)
                break;
        }
    }
    fclose(dosya);
}
void json_read_main(struct unitlist *gamei,struct creaturelist *gamec,struct herolist *gameh,struct researchlist *gamer,struct human_imperium_army *hseneryo,struct ork_legi_army *oseneryo)
{
    const char *infantryname[8] = {"piyadeler","okcular","suvariler","kusatma_makineleri","ork_dovusculeri","mizrakcilar","varg_binicileri","troller"};
    const char *heroname[9]= {"Alparslan","Fatih_Sultan_Mehmet","Mete_Han","Yavuz_Sultan_Selim","Tugrul_Bey","Goruk_Vahsi","Thruk_Kemikkiran","Vrog_Kafakiran","Ugar_Zalim"};
    const char *creaturename[11]={"Ejderha","Agri_Dagi_Devleri","Tepegoz","Karakurt","Samur","Kara_Troll","Golge_Kurtlari","Camur_Devleri","Ates_Iblisi","Makrog_Savas_Beyi","Buz_Devleri"};
    const char *researchn[4]={"savunma_ustaligi","saldiri_gelistirmesi","elit_egitim","kusatma_ustaligi"};
    const char *keyword[4]={"saldiri","savunma","saglik","kritik_sans"};
    const char *keyword2[3]={"etki_degeri","etki_turu","aciklama"};
    const char *keyword3[3]={"bonus_degeri","bonus_turu","aciklama"};
    const char *filename[4] = {"Files/unit_types.json","Files/creatures.json","Files/heroes.json","Files/research.json"};

    for(int i = 0;i < 8;i++)
    {
        for(int j = 0;j < 4;j++)
        unit_parser(gamei,filename[0],infantryname[i],keyword[j]);
    }
    for(int i = 0;i < 4;i++)
    {
        research_parser(gamer,filename[3],researchn[i]);
    }
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0;j < 3;j++)
        hero_parser(filename[2],heroname[i],keyword3[j],gameh);
    }
    for(int i = 0;i < 11;i++)
    {
        for(int j = 0;j < 3;j++)
        creature_parser(filename[1],creaturename[i],keyword2[j],gamec);
    }
    for(int i = 0; i < 8 ; i++)
    {               //seneryo.json ekle
        senerio_unit_parser(infantryname[i],hseneryo,oseneryo);
    }
     for(int i = 0; i < 9 ; i++)
    { //senerio_json,
        senerio_hero_parser(heroname[i],hseneryo,oseneryo);
    }
    for(int i = 0; i < 11 ; i++)
    {  //senerio_json,
        senerio_creature_parser(creaturename[i],hseneryo,oseneryo);
    }
    for(int i = 0; i < 4 ; i++)
    {
        senerio_research_parser(researchn[i],hseneryo,oseneryo);
    }
}
int worldTest[MAP_HEIGHT][MAP_WIDTH] =
{
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1},
    {3,3,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1}


};

typedef struct gameT
{
    SDL_Event event;
    int loopDone;
    SDL_Rect mouseRect;
    point2DT mousePoint;
    isoEngineT isoEngine;
    SDL_Rect soldierRect;

} gameT;

gameT game;
textureT tilesTex;
SDL_Rect tilesRects[NUM_ISOMETRIC_TILES];

typedef struct
{
    int currentHealth;
    int maxHealth;
    SDL_Rect healthBar;
    SDL_Rect buffBar;
    textureT soldierTex;
    int soldiersNum;
    int soldiersNum2;
    int soldiersNum3;


} unitT;

unitT soldiers[8];
unitT HeroesAndCreatures[20];
char *soldiersName[8] = {"piyade.png","okcu.png","suvari.png","kusatma_makine.png","ork_dovuscu.png","mizrakci.png","varg_binici.png","troll.png"};
char *HeroesAndCreaturesName[20] = {"ejder.png","agri_dev.png","tepegoz.png","karakurt.png","samur.png","alparslan.png","fatih.png","mete.png","yavuz.png","tugrul.png","karatroll.png","golge_kurt.png","camur_dev.png","iblis.png","makrog.png","buzdev.png","goruk.png","thruk.png","vrog.png","ugar.png",};
char *HeroesAndCreaturesNames[20] = {"Ejderha","Agri_Dagi_Devleri","Tepegoz","Karakurt","Samur","Alparslan","Fatih_Sultan_Mehmet","Mete_Han","Yavuz_Sultan_Selim","Tugrul_Bey","Kara_Troll","Golge_Kurtlari","Camur_Devleri","Ates_Iblisi","Makrog_Savas_Beyi","Buz_Devleri","Goruk_Vahsi","Thruk_Kemikkiran","Vrog_Kafakiran","Ugar_Zalim"};




void setupRect(SDL_Rect *rect,int x,int y,int w,int h)
{
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;


}
void initTileClip()
{
    int x = 0;
    int y = 0;
    int i;

    textureInit(&tilesTex,0,0,0,NULL,NULL,SDL_FLIP_NONE);

    for(i = 0; i < NUM_ISOMETRIC_TILES; ++i)
    {
        setupRect(&tilesRects[i],x,y,64,80);
        x += 64;
    }

}
TTF_Font *font;

void initFont()
{
    if (TTF_Init() == -1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(1);
    }

    font = TTF_OpenFont("LemonMilkbolditalic.otf", 10);
    if (!font)
    {
        printf("Font yuklenemedi: %s\n", TTF_GetError());
        exit(1);
    }
}
void drawUnitCount2(int number, int x, int y)
{
    SDL_Color textColor = {255, 0, 0};


    char countText[5];
    sprintf(countText, "%d",number);


    SDL_Surface* textSurface = TTF_RenderText_Solid(font, countText, textColor);
    if (!textSurface)
    {
        printf("Text render hatasi: %s\n", TTF_GetError());
        return;
    }


    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(getRenderer(), textSurface);
    SDL_FreeSurface(textSurface);
    if (!textTexture)
    {
        printf("TextTexture olusturma hatasi: %s\n", SDL_GetError());
        return;
    }


    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);


    SDL_Rect renderQuad = {x, y - 10, textWidth, textHeight};
    SDL_RenderCopy(getRenderer(), textTexture, NULL, &renderQuad);

    SDL_DestroyTexture(textTexture);
}

void init()
{
    int tileSize = 32;
    game.loopDone = 0;
    initTileClip();
    initFont();
    InitIsoEngine(&game.isoEngine,tileSize);
    IsoEngineSetMapSize(&game.isoEngine,20,20);
    game.isoEngine.scrollX = -340;
    game.isoEngine.scrollY = 440;

    if(loadTexture(&tilesTex,"isotiles.png") == 0)
    {
        printf("Resim yuklenmedi");
        exit(1);
    }

    for (int i = 0; i < 8; i++)
    {

        loadTexture(&soldiers[i].soldierTex,soldiersName[i]);
        setupRect(&game.soldierRect, 0, 0, 64, 64);
        soldiers[i].healthBar.w = 16;
        soldiers[i].healthBar.h = 4;

        for (int i = 0; i <8; i++)
        {
     //       soldiers[i].soldiersNum = soldiersNumValues[i];

            if (soldiers[i].soldiersNum < 100)
            {

                soldiers[i].soldiersNum2 = 0;
                soldiers[i].soldiersNum3 = soldiers[i].soldiersNum;
            }
            else
            {

                soldiers[i].soldiersNum2 = soldiers[i].soldiersNum / 100;
                soldiers[i].soldiersNum3 = soldiers[i].soldiersNum % 100;

            }
        }
    }
    for(int i = 0;i < 20;i++)
    {
        if(HeroesAndCreatures[i].soldiersNum==1){
        loadTexture(&HeroesAndCreatures[i].soldierTex,HeroesAndCreaturesName[i]);
        setupRect(&game.soldierRect, 0, 0, 64, 64);}
    }

    textureRenderXYClip(&tilesTex,game.mouseRect.x,game.mouseRect.y,&tilesRects[0]);

}
void drawUnitCount(unitT *soldiers, int x, int y,int i)
{
    SDL_Color textColor = {255, 0, 0};


    char countText[10];
    sprintf(countText, "%d",soldiers->soldiersNum3);


    SDL_Surface* textSurface = TTF_RenderText_Solid(font, countText, textColor);
    if (!textSurface)
    {
        printf("Text render hatasi: %s\n", TTF_GetError());
        return;
    }


    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(getRenderer(), textSurface);
    SDL_FreeSurface(textSurface);
    if (!textTexture)
    {
        printf("TextTexture olusturma hatasi: %s\n", SDL_GetError());
        return;
    }


    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);


    SDL_Rect renderQuad = {x, y - 10, textWidth, textHeight};
    SDL_RenderCopy(getRenderer(), textTexture, NULL, &renderQuad);

    SDL_DestroyTexture(textTexture);
}
void drawIsoMouse()
{
    game.mousePoint.x = (game.mouseRect.x/ TILESIZE) *  TILESIZE +13;
    game.mousePoint.y = (game.mouseRect.y/ TILESIZE) *  TILESIZE +13;

    if((game.mousePoint.x/ TILESIZE) %2)
    {
        game.mousePoint.y += TILESIZE*0.5;
    }
    textureRenderXYClip(&tilesTex,game.mousePoint.x,game.mousePoint.y,&tilesRects[0]);

}
void drawIsoMap(isoEngineT *isoEngine)
{
    int i,j;
    int tile = 4;
    point2DT point;


    for(i = 0; i < isoEngine->mapHeight; i++)
    {
        for(j = 0; j < isoEngine->mapWidth; j++)
        {
            point.x = (j * TILESIZE) + isoEngine->scrollX;
            point.y = (i * TILESIZE) + isoEngine->scrollY;

            tile = worldTest[i][j];
            Convert2dToIso(&point);

            textureRenderXYClip(&tilesTex,point.x,point.y,&tilesRects[tile]);

        }
    }

}
void drawSoldier(isoEngineT *isoEngine, int mapX, int mapY,SDL_Rect *soldierRect,textureT *soldierTex)
{
    point2DT soldierPoint;


    soldierPoint.x = (mapX * TILESIZE) + isoEngine->scrollX;
    soldierPoint.y = (mapY * TILESIZE) + isoEngine->scrollY;

    Convert2dToIso(&soldierPoint);


    SDL_Rect destRect = *soldierRect;
    destRect.x = soldierPoint.x;
    destRect.y = soldierPoint.y;

    SDL_RenderCopy(getRenderer(), soldierTex->texture, NULL, &destRect);
}
void drawUnitName(const char *name, int x, int y)
{
    SDL_Color textColor = {255,255, 255};

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, name, textColor);
    if (!textSurface)
    {
        printf("Text render error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(getRenderer(), textSurface);
    SDL_FreeSurface(textSurface);
    if (!textTexture)
    {
        printf("TextTexture creation error: %s\n", SDL_GetError());
        return;
    }

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

    SDL_Rect renderQuad = {x, y +5, textWidth, textHeight}; // Adjust y for positioning
    SDL_RenderCopy(getRenderer(), textTexture, NULL, &renderQuad);

    SDL_DestroyTexture(textTexture);
}
void drawHealthBar(unitT *unit, int x, int y)
{

    unit->healthBar.x = x;
    unit->healthBar.y = y +5;
    unit->buffBar.w = 5;
    unit->buffBar.h=5;

    if(HeroesAndCreatures[0].soldiersNum == 1 || HeroesAndCreatures[1].soldiersNum == 1 || HeroesAndCreatures[2].soldiersNum == 1 || HeroesAndCreatures[3].soldiersNum == 1 || HeroesAndCreatures[4].soldiersNum == 1 || HeroesAndCreatures[5].soldiersNum == 1 || HeroesAndCreatures[6].soldiersNum == 1 || HeroesAndCreatures[7].soldiersNum == 1 || HeroesAndCreatures[8].soldiersNum == 1 || HeroesAndCreatures[9].soldiersNum == 1)
    {

        unit->buffBar.x = x + 25;
        unit->buffBar.y = y-3;
        SDL_SetRenderDrawColor(getRenderer(),204 , 102, 0, 255);
        SDL_RenderFillRect(getRenderer(), &unit->buffBar);

    }
    if(HeroesAndCreatures[10].soldiersNum == 1 || HeroesAndCreatures[11].soldiersNum == 1 || HeroesAndCreatures[12].soldiersNum == 1 || HeroesAndCreatures[13].soldiersNum == 1 || HeroesAndCreatures[14].soldiersNum == 1 || HeroesAndCreatures[15].soldiersNum == 1 || HeroesAndCreatures[16].soldiersNum == 1 || HeroesAndCreatures[17].soldiersNum == 1 || HeroesAndCreatures[18].soldiersNum == 1 || HeroesAndCreatures[19].soldiersNum == 1)
    {
        unit->buffBar.x = x + 32;
        unit->buffBar.y = y-3;
        SDL_SetRenderDrawColor(getRenderer(),102 , 0, 102, 255);
        SDL_RenderFillRect(getRenderer(), &unit->buffBar);
    }

    float healthRatio = (float)unit->currentHealth / unit->maxHealth;
    if(healthRatio >= (float)65/100)
   {


    unit->healthBar.w = (int)(32 * healthRatio);


    SDL_SetRenderDrawColor(getRenderer(),0 , 255, 0, 255);
    SDL_RenderFillRect(getRenderer(), &unit->healthBar);
   }
   else if (healthRatio >= (float)2/10 && healthRatio < (float)65/100)
   {
       unit->healthBar.w = (int)(32 * healthRatio);


    SDL_SetRenderDrawColor(getRenderer(), 255,255, 0, 255);
    SDL_RenderFillRect(getRenderer(), &unit->healthBar);
   }
   else{
    unit->healthBar.w = (int)(32 * healthRatio);


    SDL_SetRenderDrawColor(getRenderer(), 255, 0, 0, 255);
    SDL_RenderFillRect(getRenderer(), &unit->healthBar);
   }
}
void drawUnits(unitT *soldiers)
{
    srand((unsigned int)time(NULL));
    int j;
    int i;
    int i1 = 0;
    int j1 = 0;


    for(i = 0;i < 20; i++)
    {
        if(i < 10)
        {


        int mapX = 11;
        int mapY = 2*i;

        drawSoldier(&game.isoEngine, mapX, mapY, &game.soldierRect,&HeroesAndCreatures[i].soldierTex);
        point2DT soldierPoint;
            soldierPoint.x = (mapX * TILESIZE) + game.isoEngine.scrollX;
            soldierPoint.y = (mapY * TILESIZE) + game.isoEngine.scrollY;
            Convert2dToIso(&soldierPoint);
            if(HeroesAndCreatures[i].soldiersNum==1){
            drawUnitName(HeroesAndCreaturesNames[i], soldierPoint.x, soldierPoint.y);}
        }
        else
        {
        int mapX = 8;
        int mapY = 2*i1;

        drawSoldier(&game.isoEngine, mapX, mapY, &game.soldierRect,&HeroesAndCreatures[i].soldierTex);
        point2DT soldierPoint;
            soldierPoint.x = (mapX * TILESIZE) + game.isoEngine.scrollX;
            soldierPoint.y = (mapY * TILESIZE) + game.isoEngine.scrollY;
            Convert2dToIso(&soldierPoint);
            if(HeroesAndCreatures[i].soldiersNum==1){
            drawUnitName(HeroesAndCreaturesNames[i], soldierPoint.x, soldierPoint.y);}
            i1++;

        }
    }


    for (i = 0; i < 8; i++)
    {
        if(i < 4)
        {
        for(j = 0; j < soldiers[i].soldiersNum2 ; j++)
        {
            int mapX =19-(2*i);
            int mapY = 0+j;

            drawSoldier(&game.isoEngine, mapX, mapY, &game.soldierRect,&soldiers[i].soldierTex);


            point2DT soldierPoint;
            soldierPoint.x = (mapX * TILESIZE) + game.isoEngine.scrollX;
            soldierPoint.y = (mapY * TILESIZE) + game.isoEngine.scrollY;
            Convert2dToIso(&soldierPoint);

            drawHealthBar(&soldiers[i], soldierPoint.x, soldierPoint.y);
            drawUnitCount2(100, soldierPoint.x, soldierPoint.y);

        }
        if(soldiers[i].soldiersNum3 != 0 && soldiers[i].soldiersNum != 0)
        {



            int mapX =19-(2*i);
            int mapY = j;

            drawSoldier(&game.isoEngine, mapX, mapY, &game.soldierRect,&soldiers[i].soldierTex);


            point2DT soldierPoint;
            soldierPoint.x = (mapX * TILESIZE) + game.isoEngine.scrollX;
            soldierPoint.y = (mapY * TILESIZE) + game.isoEngine.scrollY;
            Convert2dToIso(&soldierPoint);

            drawHealthBar(&soldiers[i], soldierPoint.x, soldierPoint.y);

            drawUnitCount(&soldiers[i], soldierPoint.x, soldierPoint.y,i);

        }
}
        else
        {
            j1++;
            if(soldiers[i].soldiersNum3 != 0 && soldiers[i].soldiersNum != 0)
        {

            int mapX =8-(2*j1);
            int mapY = 0;

            drawSoldier(&game.isoEngine, mapX, mapY, &game.soldierRect,&soldiers[i].soldierTex);


            point2DT soldierPoint;
            soldierPoint.x = (mapX * TILESIZE) + game.isoEngine.scrollX;
            soldierPoint.y = (mapY * TILESIZE) + game.isoEngine.scrollY;
            Convert2dToIso(&soldierPoint);

            drawHealthBar(&soldiers[i], soldierPoint.x, soldierPoint.y);

            drawUnitCount(&soldiers[i], soldierPoint.x, soldierPoint.y,i);



        }

        for(j = 0; j < soldiers[i].soldiersNum2 ; j++)
        {
            int mapX =8-(2*j1);
            int mapY = 1+j;

            drawSoldier(&game.isoEngine, mapX, mapY, &game.soldierRect,&soldiers[i].soldierTex);


            point2DT soldierPoint;
            soldierPoint.x = (mapX * TILESIZE) + game.isoEngine.scrollX;
            soldierPoint.y = (mapY * TILESIZE) + game.isoEngine.scrollY;
            Convert2dToIso(&soldierPoint);

            drawHealthBar(&soldiers[i], soldierPoint.x, soldierPoint.y);
            drawUnitCount2(100, soldierPoint.x, soldierPoint.y);


        }
        }
    }
}
void draw()
{
    SDL_SetRenderDrawColor(getRenderer(),0x3b,0x3b,0x3b,0x00);
    SDL_RenderClear(getRenderer());

    drawIsoMap(&game.isoEngine);

    drawIsoMouse();
    drawUnits(soldiers);
    SDL_RenderPresent(getRenderer());
    SDL_Delay(10);
}
void update()
{
    SDL_GetMouseState(&game.mouseRect.x,&game.mouseRect.y);

}
void updateInput()
{
    while(SDL_PollEvent(&game.event) != 0)
    {
        switch (game.event.type)
        {

        case SDL_QUIT:
            game.loopDone = 1;
            break;


        case SDL_KEYUP:
            switch(game.event.key.keysym.sym)
            case SDLK_ESCAPE:
            game.loopDone = 1;
            break;
            break;

        }
    }
}
////////////////////////////////////////////////////////////////////***main***///////////////////////////////////////////////////
int main(int argc, char *argv[])
{
setlocale(LC_ALL,"Turkish");

struct human_imperium_army hseneryo={0};
struct ork_legi_army oseneryo={0};

struct unitlist gamei={0};
struct creaturelist gamec={0};
struct researchlist gamer={0};
struct herolist gameh={0};

int infantry1=0,infantry2=0;
int heroes1=0,heroes2=0;
int creature1=0,creature2=0;

int round=1;
int hkrit=0;
int okrit=0;
float true_damage=0;
float damage=0;
float human_attack_power=0,human_defense_power=0;;
float ork_attack_power=0,ork_defense_power=0;

float piyadeler_health_max=0,okcular_health_max=0,suvariler_health_max=0,kusatma_makineleri_health_max=0;
float ork_dovusculeri_health_max=0,mizrakcilar_health_max=0,troller_health_max=0,varg_binicileri_health_max=0;
float piyadeler_health=0,okcular_health=0,suvariler_health=0,kusatma_makineleri_health=0;
float ork_dovusculeri_health=0,mizrakcilar_health=0,troller_health=0,varg_binicileri_health=0;

int piyadecr=0,okcucr=0,suvaricr=0,kusatmacr=0;
int dovuscucr=0,mizrakcr=0,vargcr=0,trollcr=0;

curl_func();
json_read_main(&gamei,&gamec,&gameh,&gamer,&hseneryo,&oseneryo);

//Birlik sayilari
infantry1=(int)hseneryo.infantryn+(int)hseneryo.archern+(int)hseneryo.cavalryn+(int)hseneryo.siege_machinen;
infantry2=(int)oseneryo.ork_fightern+(int)oseneryo.ork_spearern+(int)oseneryo.varg_ridern+(int)oseneryo.trolln;

//Kahraman sayilari
heroes1=hseneryo.alparslan+hseneryo.fatih_sm+hseneryo.mete_han+hseneryo.tugrul_bey+hseneryo.yavuz_ss;
heroes2=oseneryo.thruk_bonecrusher+oseneryo.goruk+oseneryo.vrog_headsmasher+oseneryo.ugar;

//Canavar sayilari
creature1=hseneryo.dragon+hseneryo.agri_mountain_giants+hseneryo.samur+hseneryo.blackwolf+hseneryo.goliath;
creature2=oseneryo.dark_troll+oseneryo.shadow_wolfs+oseneryo.mud_giants+oseneryo.fire_demon+oseneryo.makrog;

//Max canlar

piyadeler_health_max=(hseneryo.infantryn*gamei.units[0].saglik);
okcular_health_max=(hseneryo.archern*gamei.units[1].saglik);
suvariler_health_max=(hseneryo.cavalryn*gamei.units[2].saglik);
kusatma_makineleri_health_max=(hseneryo.siege_machinen*gamei.units[3].saglik);
ork_dovusculeri_health_max=(oseneryo.ork_fightern*gamei.units[4].saglik);
mizrakcilar_health_max=(oseneryo.ork_spearern*gamei.units[5].saglik);
troller_health_max=(oseneryo.trolln*gamei.units[6].saglik);
varg_binicileri_health_max=(oseneryo.varg_ridern*gamei.units[7].saglik);

//Guncel canlar
piyadeler_health=piyadeler_health_max;
okcular_health=okcular_health_max;
suvariler_health=suvariler_health_max;
kusatma_makineleri_health=kusatma_makineleri_health_max;
ork_dovusculeri_health=ork_dovusculeri_health_max;
mizrakcilar_health=mizrakcilar_health_max;
troller_health=troller_health_max;
varg_binicileri_health=varg_binicileri_health_max;

//Kritik vurus round sayisi


if(strcmp(hseneryo.h_arastirma,"savunma_ustaligi")==0)
{
    if(hseneryo.h_arastirmalvl==1){
        gamei.units[0].savunma=gamei.units[0].savunma*(100+gamer.ss[0].oran)/100;
        gamei.units[1].savunma=gamei.units[1].savunma*(100+gamer.ss[0].oran)/100;
        gamei.units[2].savunma=gamei.units[2].savunma*(100+gamer.ss[0].oran)/100;
        gamei.units[3].savunma=gamei.units[3].savunma*(100+gamer.ss[0].oran)/100;}
    if(hseneryo.h_arastirmalvl==2){
        gamei.units[0].savunma=gamei.units[0].savunma*(100+gamer.ss[1].oran)/100;
        gamei.units[1].savunma=gamei.units[1].savunma*(100+gamer.ss[1].oran)/100;
        gamei.units[2].savunma=gamei.units[2].savunma*(100+gamer.ss[1].oran)/100;
        gamei.units[3].savunma=gamei.units[3].savunma*(100+gamer.ss[1].oran)/100;}
    if(hseneryo.h_arastirmalvl==3){
        gamei.units[0].savunma=gamei.units[0].savunma*(100+gamer.ss[2].oran)/100;
        gamei.units[1].savunma=gamei.units[1].savunma*(100+gamer.ss[2].oran)/100;
        gamei.units[2].savunma=gamei.units[2].savunma*(100+gamer.ss[2].oran)/100;
        gamei.units[3].savunma=gamei.units[3].savunma*(100+gamer.ss[2].oran)/100;}
}
if(strcmp(hseneryo.h_arastirma,"saldiri_gelistirmesi")==0)
{
    if(hseneryo.h_arastirmalvl==1){
        gamei.units[0].saldiri=gamei.units[0].saldiri*(100+gamer.ss[3].oran)/100;
        gamei.units[1].saldiri=gamei.units[1].saldiri*(100+gamer.ss[3].oran)/100;
        gamei.units[2].saldiri=gamei.units[2].saldiri*(100+gamer.ss[3].oran)/100;
        gamei.units[3].saldiri=gamei.units[3].saldiri*(100+gamer.ss[3].oran)/100;}
    if(hseneryo.h_arastirmalvl==2){
        gamei.units[0].saldiri=gamei.units[0].saldiri*(100+gamer.ss[4].oran)/100;
        gamei.units[1].saldiri=gamei.units[1].saldiri*(100+gamer.ss[4].oran)/100;
        gamei.units[2].saldiri=gamei.units[2].saldiri*(100+gamer.ss[4].oran)/100;
        gamei.units[3].saldiri=gamei.units[3].saldiri*(100+gamer.ss[4].oran)/100;}
    if(hseneryo.h_arastirmalvl==3){
        gamei.units[0].saldiri=gamei.units[0].saldiri*(100+gamer.ss[5].oran)/100;
        gamei.units[1].saldiri=gamei.units[1].saldiri*(100+gamer.ss[5].oran)/100;
        gamei.units[2].saldiri=gamei.units[2].saldiri*(100+gamer.ss[5].oran)/100;
        gamei.units[3].saldiri=gamei.units[3].saldiri*(100+gamer.ss[5].oran)/100;}
}
if(strcmp(hseneryo.h_arastirma,"elit_egitim")==0)
{
    if(hseneryo.h_arastirmalvl==1)
    hkrit=gamer.ss[6].oran;
    if(hseneryo.h_arastirmalvl==2)
    hkrit=gamer.ss[7].oran;
    if(hseneryo.h_arastirmalvl==3)
    hkrit=gamer.ss[8].oran;
}
if(strcmp(hseneryo.h_arastirma,"kusatma_ustaligi")==0)
{
    if(hseneryo.h_arastirmalvl==1)
       gamei.units[3].saldiri=gamei.units[3].saldiri*(100+gamer.ss[9].oran)/100;
    if(hseneryo.h_arastirmalvl==2)
       gamei.units[3].saldiri=gamei.units[3].saldiri*(100+gamer.ss[10].oran)/100;
    if(hseneryo.h_arastirmalvl==3)
       gamei.units[3].saldiri=gamei.units[3].saldiri*(100+gamer.ss[11].oran)/100;
}
/////////////ork
if(strcmp(oseneryo.o_arastirma,"savunma_ustaligi")==0)
{
    if(oseneryo.o_arastirmalvl==1){
        gamei.units[4].savunma=gamei.units[4].savunma*(100+gamer.ss[0].oran)/100;
        gamei.units[5].savunma=gamei.units[5].savunma*(100+gamer.ss[0].oran)/100;
        gamei.units[6].savunma=gamei.units[6].savunma*(100+gamer.ss[0].oran)/100;
        gamei.units[7].savunma=gamei.units[7].savunma*(100+gamer.ss[0].oran)/100;}
    if(oseneryo.o_arastirmalvl==2){
        gamei.units[4].savunma=gamei.units[4].savunma*(100+gamer.ss[1].oran)/100;
        gamei.units[5].savunma=gamei.units[5].savunma*(100+gamer.ss[1].oran)/100;
        gamei.units[6].savunma=gamei.units[6].savunma*(100+gamer.ss[1].oran)/100;
        gamei.units[7].savunma=gamei.units[7].savunma*(100+gamer.ss[1].oran)/100;}
    if(oseneryo.o_arastirmalvl==3){
        gamei.units[4].savunma=gamei.units[4].savunma*(100+gamer.ss[2].oran)/100;
        gamei.units[5].savunma=gamei.units[5].savunma*(100+gamer.ss[2].oran)/100;
        gamei.units[6].savunma=gamei.units[6].savunma*(100+gamer.ss[2].oran)/100;
        gamei.units[7].savunma=gamei.units[7].savunma*(100+gamer.ss[2].oran)/100;}
}
if(strcmp(oseneryo.o_arastirma,"saldiri_gelistirmesi")==0)
{
    if(oseneryo.o_arastirmalvl==1){
        gamei.units[4].saldiri=gamei.units[4].saldiri*(100+gamer.ss[3].oran)/100;
        gamei.units[5].saldiri=gamei.units[5].saldiri*(100+gamer.ss[3].oran)/100;
        gamei.units[6].saldiri=gamei.units[6].saldiri*(100+gamer.ss[3].oran)/100;
        gamei.units[7].saldiri=gamei.units[7].saldiri*(100+gamer.ss[3].oran)/100;}
    if(oseneryo.o_arastirmalvl==2){
        gamei.units[4].saldiri=gamei.units[4].saldiri*(100+gamer.ss[4].oran)/100;
        gamei.units[5].saldiri=gamei.units[5].saldiri*(100+gamer.ss[4].oran)/100;
        gamei.units[6].saldiri=gamei.units[6].saldiri*(100+gamer.ss[4].oran)/100;
        gamei.units[7].saldiri=gamei.units[7].saldiri*(100+gamer.ss[4].oran)/100;}
    if(oseneryo.o_arastirmalvl==3){
        gamei.units[4].saldiri=gamei.units[4].saldiri*(100+gamer.ss[5].oran)/100;
        gamei.units[5].saldiri=gamei.units[5].saldiri*(100+gamer.ss[5].oran)/100;
        gamei.units[6].saldiri=gamei.units[6].saldiri*(100+gamer.ss[5].oran)/100;
        gamei.units[7].saldiri=gamei.units[7].saldiri*(100+gamer.ss[5].oran)/100;}
}
if(strcmp(oseneryo.o_arastirma,"elit_egitim")==0)
{
    if(hseneryo.h_arastirmalvl==1){
    okrit=gamer.ss[6].oran;
    if(hseneryo.h_arastirmalvl==2)
    okrit=gamer.ss[7].oran;
    if(hseneryo.h_arastirmalvl==3)
    okrit=gamer.ss[8].oran;
}
}
for(int i=0;i<5;i++){
if(i==0&&hseneryo.alparslan==0)
continue;
if(i==1&&hseneryo.fatih_sm==0)
continue;
if(i==2&&hseneryo.mete_han==0)
continue;
if(i==3&&hseneryo.yavuz_ss==0)
continue;
if(i==4&&hseneryo.tugrul_bey==0)
continue;

   if(strcmp(gameh.hs[i].aciklama,"piyade")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        gamei.units[0].saldiri+=gamei.units[0].saldiri*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        gamei.units[0].savunma+=gamei.units[0].savunma*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0)
        gamei.units[0].kritik_sans=gamei.units[0].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}
    if(strcmp(gameh.hs[i].aciklama,"okcu")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        gamei.units[1].saldiri+=gamei.units[1].saldiri*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        gamei.units[1].savunma+=gamei.units[1].savunma*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0)
        gamei.units[1].kritik_sans=gamei.units[1].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}
    if(strcmp(gameh.hs[i].aciklama,"suvari")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        gamei.units[2].saldiri+=gamei.units[2].saldiri*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        gamei.units[2].savunma+=gamei.units[2].savunma*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0)
        gamei.units[2].kritik_sans=gamei.units[2].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}
    if(strcmp(gameh.hs[i].aciklama,"kusatma_makine")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        gamei.units[3].saldiri+=gamei.units[3].saldiri*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        gamei.units[3].savunma+=gamei.units[3].savunma*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0)
        gamei.units[3].kritik_sans=gamei.units[3].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}
    if(strcmp(gameh.hs[i].aciklama,"tum_birim")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        {gamei.units[0].saldiri+=gamei.units[0].saldiri*gameh.hs[i].bonus_degeri/100;
        gamei.units[1].saldiri+=gamei.units[1].saldiri*gameh.hs[i].bonus_degeri/100;
        gamei.units[2].saldiri+=gamei.units[2].saldiri*gameh.hs[i].bonus_degeri/100;
        gamei.units[3].saldiri+=gamei.units[3].saldiri*gameh.hs[i].bonus_degeri/100;}
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        {gamei.units[0].savunma+=gamei.units[0].savunma*gameh.hs[i].bonus_degeri/100;
        gamei.units[1].savunma+=gamei.units[1].savunma*gameh.hs[i].bonus_degeri/100;
        gamei.units[2].savunma+=gamei.units[2].savunma*gameh.hs[i].bonus_degeri/100;
        gamei.units[3].savunma+=gamei.units[3].savunma*gameh.hs[i].bonus_degeri/100;}
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0){
        gamei.units[0].kritik_sans=gamei.units[0].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;
        gamei.units[1].kritik_sans=gamei.units[1].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;
        gamei.units[2].kritik_sans=gamei.units[2].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;
        gamei.units[3].kritik_sans=gamei.units[3].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}}
}

for(int i=5;i<9;i++){
if(i==5&&oseneryo.goruk==0)
continue;
if(i==6&&oseneryo.thruk_bonecrusher==0)
continue;
if(i==7&&oseneryo.vrog_headsmasher==0)
continue;
if(i==8&&oseneryo.ugar==0)
continue;


   if(strcmp(gameh.hs[i].aciklama,"ork_dovuscu")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        gamei.units[4].saldiri+=gamei.units[4].saldiri*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        gamei.units[4].savunma+=gamei.units[4].savunma*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0)
        gamei.units[4].kritik_sans=gamei.units[4].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}
    if(strcmp(gameh.hs[i].aciklama,"mizrakci")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        gamei.units[5].saldiri+=gamei.units[5].saldiri*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        gamei.units[5].savunma+=gamei.units[5].savunma*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0)
        gamei.units[5].kritik_sans=gamei.units[5].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}
    if(strcmp(gameh.hs[i].aciklama,"varg_binici")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        gamei.units[6].saldiri+=gamei.units[6].saldiri*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        gamei.units[6].savunma+=gamei.units[6].savunma*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0)
        gamei.units[6].kritik_sans=gamei.units[6].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}
    if(strcmp(gameh.hs[i].aciklama,"troll")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        gamei.units[7].saldiri+=gamei.units[7].saldiri*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        gamei.units[7].savunma+=gamei.units[7].savunma*gameh.hs[i].bonus_degeri/100;
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0)
        gamei.units[7].kritik_sans=gamei.units[7].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}
    if(strcmp(gameh.hs[i].aciklama,"tum_birim")==0)
    {
        if(strcmp(gameh.hs[i].bonus_turu,"saldiri")==0)
        {gamei.units[4].saldiri+=gamei.units[4].saldiri*gameh.hs[i].bonus_degeri/100;
        gamei.units[5].saldiri+=gamei.units[5].saldiri*gameh.hs[i].bonus_degeri/100;
        gamei.units[6].saldiri+=gamei.units[6].saldiri*gameh.hs[i].bonus_degeri/100;
        gamei.units[7].saldiri+=gamei.units[7].saldiri*gameh.hs[i].bonus_degeri/100;}
        if(strcmp(gameh.hs[i].bonus_turu,"savunma")==0)
        {gamei.units[4].savunma+=gamei.units[4].savunma*gameh.hs[i].bonus_degeri/100;
        gamei.units[5].savunma+=gamei.units[5].savunma*gameh.hs[i].bonus_degeri/100;
        gamei.units[6].savunma+=gamei.units[6].savunma*gameh.hs[i].bonus_degeri/100;
        gamei.units[7].savunma+=gamei.units[7].savunma*gameh.hs[i].bonus_degeri/100;}
        if(strcmp(gameh.hs[i].bonus_turu,"kritik_sans")==0){
        gamei.units[4].kritik_sans=gamei.units[4].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;
        gamei.units[5].kritik_sans=gamei.units[5].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;
        gamei.units[6].kritik_sans=gamei.units[6].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;
        gamei.units[7].kritik_sans=gamei.units[7].kritik_sans*(100+gameh.hs[i].bonus_degeri)/100;}}
}
/////creature
for(int i=0;i<5;i++){
if(i==0&&hseneryo.dragon==0)
continue;
if(i==1&&hseneryo.agri_mountain_giants==0)
continue;
if(i==2&&hseneryo.goliath==0)
continue;
if(i==3&&hseneryo.blackwolf==0)
continue;
if(i==4&&hseneryo.samur==0)
continue;


   if(strcmp(gamec.cs[i].aciklama,"piyade")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        gamei.units[0].saldiri+=gamei.units[0].saldiri*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        gamei.units[0].savunma+=gamei.units[0].savunma*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0)
        gamei.units[0].kritik_sans=gamei.units[0].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}
    if(strcmp(gamec.cs[i].aciklama,"okcu")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        gamei.units[1].saldiri+=gamei.units[1].saldiri*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        gamei.units[1].savunma+=gamei.units[1].savunma*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0)
        gamei.units[1].kritik_sans=gamei.units[1].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}
    if(strcmp(gamec.cs[i].aciklama,"suvari")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        gamei.units[2].saldiri+=gamei.units[2].saldiri*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        gamei.units[2].savunma+=gamei.units[2].savunma*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0)
        gamei.units[2].kritik_sans=gamei.units[2].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}
    if(strcmp(gamec.cs[i].aciklama,"kusatma_makine")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        gamei.units[3].saldiri+=gamei.units[3].saldiri*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        gamei.units[3].savunma+=gamei.units[3].savunma*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0)
        gamei.units[3].kritik_sans=gamei.units[3].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}
    if(strcmp(gamec.cs[i].aciklama,"tum_birim")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        {gamei.units[0].saldiri+=gamei.units[0].saldiri*gamec.cs[i].etki_degeri/100;
        gamei.units[1].saldiri+=gamei.units[5].saldiri*gamec.cs[i].etki_degeri/100;
        gamei.units[2].saldiri+=gamei.units[6].saldiri*gamec.cs[i].etki_degeri/100;
        gamei.units[3].saldiri+=gamei.units[7].saldiri*gamec.cs[i].etki_degeri/100;}
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        {gamei.units[0].savunma+=gamei.units[0].savunma*gamec.cs[i].etki_degeri/100;
        gamei.units[1].savunma+=gamei.units[1].savunma*gamec.cs[i].etki_degeri/100;
        gamei.units[2].savunma+=gamei.units[2].savunma*gamec.cs[i].etki_degeri/100;
        gamei.units[3].savunma+=gamei.units[3].savunma*gamec.cs[i].etki_degeri/100;}
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0){
        gamei.units[0].kritik_sans=gamei.units[0].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;
        gamei.units[1].kritik_sans=gamei.units[1].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;
        gamei.units[2].kritik_sans=gamei.units[2].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;
        gamei.units[3].kritik_sans=gamei.units[3].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}}
}

for(int i=5;i<11;i++){
if(i==5&&oseneryo.dark_troll==0)
continue;
if(i==6&&oseneryo.shadow_wolfs==0)
continue;
if(i==7&&oseneryo.mud_giants==0)
continue;
if(i==8&&oseneryo.fire_demon==0)
continue;
if(i==9&&oseneryo.makrog==0)
continue;
if(i==10&&oseneryo.ice_giants==0)
continue;

   if(strcmp(gamec.cs[i].aciklama,"ork_dovuscu")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        gamei.units[4].saldiri+=gamei.units[4].saldiri*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        gamei.units[4].savunma+=gamei.units[4].savunma*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0)
        gamei.units[4].kritik_sans=gamei.units[4].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}
    if(strcmp(gamec.cs[i].aciklama,"mizrakci")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        gamei.units[5].saldiri+=gamei.units[5].saldiri*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        gamei.units[5].savunma+=gamei.units[5].savunma*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0)
        gamei.units[5].kritik_sans=gamei.units[5].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}
    if(strcmp(gamec.cs[i].aciklama,"varg_binici")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        gamei.units[6].saldiri+=gamei.units[6].saldiri*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        gamei.units[6].savunma+=gamei.units[6].savunma*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0)
        gamei.units[6].kritik_sans=gamei.units[6].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}
    if(strcmp(gamec.cs[i].aciklama,"troll")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        gamei.units[7].saldiri+=gamei.units[7].saldiri*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        gamei.units[7].savunma+=gamei.units[7].savunma*gamec.cs[i].etki_degeri/100;
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0)
        gamei.units[7].kritik_sans=gamei.units[7].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}
    if(strcmp(gamec.cs[i].aciklama,"tum_birim")==0)
    {
        if(strcmp(gamec.cs[i].etki_turu,"saldiri")==0)
        {gamei.units[4].saldiri+=gamei.units[4].saldiri*gamec.cs[i].etki_degeri/100;
        gamei.units[5].saldiri+=gamei.units[5].saldiri*gamec.cs[i].etki_degeri/100;
        gamei.units[6].saldiri+=gamei.units[6].saldiri*gamec.cs[i].etki_degeri/100;
        gamei.units[7].saldiri+=gamei.units[7].saldiri*gamec.cs[i].etki_degeri/100;}
        if(strcmp(gamec.cs[i].etki_turu,"savunma")==0)
        {gamei.units[4].savunma+=gamei.units[4].savunma*gamec.cs[i].etki_degeri/100;
        gamei.units[5].savunma+=gamei.units[5].savunma*gamec.cs[i].etki_degeri/100;
        gamei.units[6].savunma+=gamei.units[6].savunma*gamec.cs[i].etki_degeri/100;
        gamei.units[7].savunma+=gamei.units[7].savunma*gamec.cs[i].etki_degeri/100;}
        if(strcmp(gamec.cs[i].etki_turu,"kritik_sans")==0){
        gamei.units[4].kritik_sans=gamei.units[4].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;
        gamei.units[5].kritik_sans=gamei.units[5].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;
        gamei.units[6].kritik_sans=gamei.units[6].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;
        gamei.units[7].kritik_sans=gamei.units[7].kritik_sans*(100+gamec.cs[i].etki_degeri)/100;}}
}

if(gamei.units[0].kritik_sans!=0)
piyadecr=(100/(int)gamei.units[0].kritik_sans);
if(gamei.units[1].kritik_sans!=0)
okcucr=(100/(int)gamei.units[1].kritik_sans);
if(gamei.units[2].kritik_sans!=0)
suvaricr=(100/(int)gamei.units[2].kritik_sans);
if(gamei.units[3].kritik_sans!=0)
kusatmacr=(100/(int)gamei.units[3].kritik_sans);
if(gamei.units[4].kritik_sans!=0)
dovuscucr=(100/(int)gamei.units[4].kritik_sans);
if(gamei.units[5].kritik_sans!=0)
mizrakcr=(100/(int)gamei.units[5].kritik_sans);
if(gamei.units[6].kritik_sans!=0)
vargcr=(100/(int)gamei.units[6].kritik_sans);
if(gamei.units[7].kritik_sans!=0)
trollcr=(100/(int)gamei.units[7].kritik_sans);

if(hseneryo.dragon==1)
HeroesAndCreatures[0].soldiersNum=1;
if(hseneryo.agri_mountain_giants==1)
HeroesAndCreatures[1].soldiersNum=1;
if(hseneryo.goliath==1)
HeroesAndCreatures[2].soldiersNum=1;
if(hseneryo.blackwolf==1)
HeroesAndCreatures[3].soldiersNum=1;
if(hseneryo.samur==1)
HeroesAndCreatures[4].soldiersNum=1;
if(hseneryo.alparslan==1)
HeroesAndCreatures[5].soldiersNum=1;
if(hseneryo.fatih_sm==1)
HeroesAndCreatures[6].soldiersNum=1;
if(hseneryo.mete_han==1)
HeroesAndCreatures[7].soldiersNum=1;
if(hseneryo.yavuz_ss==1)
HeroesAndCreatures[8].soldiersNum=1;
if(hseneryo.tugrul_bey==1)
HeroesAndCreatures[9].soldiersNum=1;
if(oseneryo.dark_troll==1)
HeroesAndCreatures[10].soldiersNum=1;
if(oseneryo.shadow_wolfs==1)
HeroesAndCreatures[11].soldiersNum=1;
if(oseneryo.mud_giants==1)
HeroesAndCreatures[12].soldiersNum=1;
if(oseneryo.fire_demon==1)
HeroesAndCreatures[13].soldiersNum=1;
if(oseneryo.makrog==1)
HeroesAndCreatures[14].soldiersNum=1;
if(oseneryo.ice_giants==1)
HeroesAndCreatures[15].soldiersNum=1;
if(oseneryo.goruk==1)
HeroesAndCreatures[16].soldiersNum=1;
if(oseneryo.thruk_bonecrusher==1)
HeroesAndCreatures[17].soldiersNum=1;
if(oseneryo.vrog_headsmasher==1)
HeroesAndCreatures[18].soldiersNum=1;
if(oseneryo.ugar==1)
HeroesAndCreatures[19].soldiersNum=1;



FILE *log =fopen("Savas_sim.txt","w+");
if(log==NULL)
    printf("Dosya acilamadi!");
if(hseneryo.infantryn==0&&hseneryo.archern==0&&hseneryo.cavalryn==0&&hseneryo.siege_machinen==0&&oseneryo.ork_fightern==0&&oseneryo.ork_spearern==0&&oseneryo.trolln==0&&oseneryo.varg_ridern==0)
{fprintf(stderr, "\nUYARI:Indirilen dosyanin icerigi okunamadi. Lutfen indirilen dosya'yi ve url'yi kontrol ediniz.");}
else{
fprintf(stderr,"Savas sonucu Savas_sim.txt adli dosyaya yazdirildi.\n");
fprintf(log,"Insan Imparatorlugu %d birlige , %d kahramana , %d canavara sahip",infantry1,heroes1,creature1);
fprintf(log,"\n----------Birlikler---------- ");
if(hseneryo.infantryn>0)fprintf(log,"\nPiyadeler: %d",hseneryo.infantryn);
if(hseneryo.archern>0)fprintf(log,"\nOkcular: %d",hseneryo.archern);
if(hseneryo.cavalryn>0)fprintf(log,"\nSuvariler: %d",hseneryo.cavalryn);
if(hseneryo.siege_machinen>0)fprintf(log,"\nKusatma Makineleri: %d",hseneryo.siege_machinen);
fprintf(log,"\nKahramanlar: ");
if(hseneryo.alparslan==0&&hseneryo.fatih_sm==0&&hseneryo.mete_han==0&&hseneryo.yavuz_ss==0&&hseneryo.tugrul_bey==0)fprintf(log,"Yok");
if(hseneryo.alparslan==1)fprintf(log," Alparslan ");
if(hseneryo.fatih_sm==1)fprintf(log," Fatih Sultan Mehmet ");
if(hseneryo.mete_han==1)fprintf(log," Mete Han ");
if(hseneryo.yavuz_ss==1)fprintf(log," Yavuz Sultan Selim ");
if(hseneryo.tugrul_bey==1)fprintf(log," Tugrul Bey ");
fprintf(log,"\nCanavarlar: ");
if(hseneryo.dragon==0&&hseneryo.agri_mountain_giants==0&&hseneryo.goliath==0&&hseneryo.blackwolf==0&&hseneryo.samur==0)fprintf(log,"Yok");
if(hseneryo.dragon==1)fprintf(log," Ejderha ");
if(hseneryo.agri_mountain_giants==1)fprintf(log," Agri Dagi Devi ");
if(hseneryo.goliath==1)fprintf(log," Tepegoz ");
if(hseneryo.blackwolf==1)fprintf(log," Karakurt ");
if(hseneryo.samur==1)fprintf(log," Samur ");
fprintf(log,"\nArastirma Ozelligi: ");
if(hseneryo.h_arastirmalvl<0&&hseneryo.h_arastirmalvl>3)fprintf(log,"Yok");
if(strcmp(hseneryo.h_arastirma,"savunma_ustaligi")==0)fprintf(log,"%d.Seviye Savunma Ustaligi",hseneryo.h_arastirmalvl);
if(strcmp(hseneryo.h_arastirma,"saldiri_gelistirmesi")==0)fprintf(log,"%d.Seviye Saldiri Gelistirmesi",hseneryo.h_arastirmalvl);
if(strcmp(hseneryo.h_arastirma,"elit_egitim")==0)fprintf(log,"%d.Seviye Elit Egitim",hseneryo.h_arastirmalvl);
if(strcmp(hseneryo.h_arastirma,"kusatma_ustaligi")==0)fprintf(log,"%d.Seviye Kusatma Ustaligi",hseneryo.h_arastirmalvl);

fprintf(log,"\n\nOrk legi %d birlige , %d kahramana , %d canavara sahip",infantry2,heroes2,creature2);
fprintf(log,"\n----------Birlikler---------- ");
if(oseneryo.ork_fightern>0)fprintf(log,"\nOrk Dovusculeri: %d",oseneryo.ork_fightern);
if(oseneryo.ork_spearern>0)fprintf(log,"\nMizrakcilar: %d",oseneryo.ork_spearern);
if(oseneryo.varg_ridern>0)fprintf(log,"\nVarg Binicileri: %d",oseneryo.varg_ridern);
if(oseneryo.trolln>0)fprintf(log,"\nTroller: %d",oseneryo.trolln);
fprintf(log,"\nKahramanlar: ");
if(oseneryo.goruk==0&&oseneryo.thruk_bonecrusher==0&&oseneryo.vrog_headsmasher==0&&oseneryo.ugar==0)fprintf(log,"Yok");
if(oseneryo.goruk==1)fprintf(log," Goruk Vahsi ");
if(oseneryo.thruk_bonecrusher==1)fprintf(log," Thruk Kemikkiran ");
if(oseneryo.vrog_headsmasher==1)fprintf(log," Vrog Kafakiran ");
if(oseneryo.ugar==1)fprintf(log," Ugar Zalim ");
fprintf(log,"\nCanavarlar: ");
if(oseneryo.dark_troll==0&&oseneryo.shadow_wolfs==0&&oseneryo.mud_giants==0&&oseneryo.fire_demon==0&&oseneryo.ice_giants==0&&oseneryo.makrog==0)fprintf(log,"Yok");
if(oseneryo.dark_troll==1)fprintf(log," Kara Troll ");
if(oseneryo.shadow_wolfs==1)fprintf(log," Golge Kurtlari ");
if(oseneryo.mud_giants==1)fprintf(log," Camur Devleri ");
if(oseneryo.fire_demon==1)fprintf(log," Ates Iblisi ");
if(oseneryo.makrog==1)fprintf(log," Makrog Savas Beyi ");
if(oseneryo.ice_giants==1)fprintf(log," Buz Devi ");
fprintf(log,"\nArastirma Ozelligi: ");
if(oseneryo.o_arastirmalvl<0&&oseneryo.o_arastirmalvl>3)fprintf(log,"Yok");
if(strcmp(oseneryo.o_arastirma,"savunma_ustaligi")==0)fprintf(log,"%d.Seviye Savunma Ustaligi",oseneryo.o_arastirmalvl);
if(strcmp(oseneryo.o_arastirma,"saldiri_gelistirmesi")==0)fprintf(log,"%d.Seviye Saldiri Gelistirmesi",oseneryo.o_arastirmalvl);
if(strcmp(oseneryo.o_arastirma,"elit_egitim")==0)fprintf(log,"%d.Seviye  Elit Egitim",oseneryo.o_arastirmalvl);
fprintf(log,"\n\n///////////////Savas Baslangici///////////////\n\n");}
while(infantry1>0&&infantry2>0)
{
human_attack_power=(hseneryo.infantryn*gamei.units[0].saldiri)+(hseneryo.archern*gamei.units[1].saldiri)+(hseneryo.cavalryn*gamei.units[2].saldiri)+(hseneryo.siege_machinen*gamei.units[3].saldiri);
ork_attack_power=(oseneryo.ork_fightern*gamei.units[4].saldiri)+(oseneryo.ork_spearern*gamei.units[5].saldiri)+(oseneryo.trolln*gamei.units[6].saldiri)+(oseneryo.varg_ridern*gamei.units[7].saldiri);
human_defense_power=(hseneryo.infantryn*gamei.units[0].savunma)+(hseneryo.archern*gamei.units[1].savunma)+(hseneryo.cavalryn*gamei.units[2].savunma)+(hseneryo.siege_machinen*gamei.units[3].savunma);
ork_defense_power=(oseneryo.ork_fightern*gamei.units[4].savunma)+(oseneryo.ork_spearern*gamei.units[5].savunma)+(oseneryo.trolln*gamei.units[6].savunma)+(oseneryo.varg_ridern*gamei.units[7].savunma);

if(round/5>0)
{for(int i=0;i<(round/5);i++)
    {
  human_attack_power=human_attack_power*(float)(9.0/10.0);
  human_defense_power=human_defense_power*(float)(9.0/10.0);
  ork_attack_power=ork_attack_power*(float)(9.0/10.0);
  ork_defense_power=ork_defense_power*(float)(9.0/10.0);
    }
}

soldiers[0].soldiersNum=hseneryo.infantryn;
soldiers[1].soldiersNum=hseneryo.archern;
soldiers[2].soldiersNum=hseneryo.cavalryn;
soldiers[3].soldiersNum=hseneryo.siege_machinen;
soldiers[4].soldiersNum=oseneryo.ork_fightern;
soldiers[5].soldiersNum=oseneryo.ork_spearern;
soldiers[6].soldiersNum=oseneryo.varg_ridern;
soldiers[7].soldiersNum=oseneryo.trolln;

soldiers[0].currentHealth=piyadeler_health;
soldiers[1].currentHealth=okcular_health;
soldiers[2].currentHealth=suvariler_health;
soldiers[3].currentHealth=kusatma_makineleri_health;
soldiers[4].currentHealth=ork_dovusculeri_health;
soldiers[5].currentHealth=mizrakcilar_health;
soldiers[6].currentHealth=varg_binicileri_health;
soldiers[7].currentHealth=troller_health;

soldiers[0].maxHealth = piyadeler_health_max;
soldiers[1].maxHealth = okcular_health_max;
soldiers[2].maxHealth = suvariler_health_max;
soldiers[3].maxHealth = kusatma_makineleri_health_max;
soldiers[4].maxHealth = ork_dovusculeri_health_max;
soldiers[5].maxHealth = mizrakcilar_health_max;
soldiers[6].maxHealth = varg_binicileri_health_max;
soldiers[7].maxHealth = troller_health_max;



initSDL("Savas Similasyonu");
    init();


    while(!game.loopDone)
    {
        update();
        updateInput();
        draw();
    }
    closeDownSDL();
    TTF_CloseFont(font);
    TTF_Quit();



    fprintf(log,"%d.tur: ",round);
    if(round%5==0)
    fprintf(log,"Butun birimler yorulmaya basladi. Birimlerin gucleri %%10 azaldi\n");
 if((round%2)==1)
    {
    //insan turu ork savunma
    if(piyadecr!=0){
    if(round!=0&&round%piyadecr==0&&gamei.units[0].kritik_sans!=0)
        human_attack_power+=(float)(hseneryo.infantryn*gamei.units[0].saldiri*(150+hkrit)/100);}
    if(round!=0&&round%okcucr==0&&gamei.units[1].kritik_sans!=0)
    if(okcucr!=0){
        human_attack_power+=(float)(hseneryo.archern*gamei.units[1].saldiri*(150+hkrit)/100);}
    if(suvaricr!=0){
    if(round!=0&&round%suvaricr==0&&gamei.units[2].kritik_sans!=0)
        human_attack_power+=(float)(hseneryo.cavalryn*gamei.units[2].saldiri*(150+hkrit)/100);}
    if(kusatmacr!=0){
    if(round!=0&&round%kusatmacr==0&&gamei.units[3].kritik_sans!=0)
        human_attack_power+=(float)(hseneryo.siege_machinen*gamei.units[3].saldiri*(150+hkrit)/100);}


        true_damage=human_attack_power*(float)(1-ork_defense_power/human_attack_power);

        if(true_damage<=0)
            fprintf(log,"tur bos gecildi. Insan irki hasar vuramadi.\n");
        else
        {
            damage=(float)((oseneryo.ork_fightern*gamei.units[4].savunma/ork_defense_power)*true_damage);

            if(damage>0)
          {
              if (ork_dovusculeri_health<=damage) {
                fprintf(log,"Butun ork dovusculeri oldu.\n");
            oseneryo.ork_fightern = 0;
            ork_dovusculeri_health=0;
            }  else {
                ork_dovusculeri_health-=damage;
            fprintf(log,"Ork dovusculeri %f hasar aldi. Guncel can degerleri %%%f\n",damage,(ork_dovusculeri_health/ork_dovusculeri_health_max*100));
                    }
              damage=0;
          }
            damage=(float)((oseneryo.ork_spearern*gamei.units[5].savunma/ork_defense_power)*true_damage);
            if(damage>0)
          {
              if (mizrakcilar_health<damage) {
            oseneryo.ork_spearern = 0;
            mizrakcilar_health=0;
            fprintf(log,"Butun ork mizrakcilari oldu.\n");
            }  else {
                mizrakcilar_health-=damage;
            fprintf(log,"Ork mizrakcilari %f hasar aldi. Guncel can degerleri %%%f\n",damage,mizrakcilar_health/mizrakcilar_health_max*100);
                }
              damage=0;
          }
            damage=(float)((oseneryo.varg_ridern*gamei.units[6].savunma/ork_defense_power)*true_damage);
            if(damage>0)
          {
            if (varg_binicileri_health<=damage) {
                oseneryo.varg_ridern = 0;
                varg_binicileri_health=0;
                fprintf(log,"Butun varg binicileri oldu.\n");
            } else {
                                varg_binicileri_health-=damage;
                fprintf(log,"Varg binicileri %f hasar aldi. Guncel can degerleri %%%f\n",damage,varg_binicileri_health/varg_binicileri_health_max*100);
            }
              damage=0;
          }
            damage=(float)((oseneryo.trolln*gamei.units[7].savunma/ork_defense_power)*true_damage);
            if(damage>0)
          {
            if (troller_health<=damage) {
                oseneryo.trolln = 0;
                troller_health=0;
                fprintf(log,"Butun troller oldu.\n");
            } else {
                                troller_health-=damage;
                fprintf(log,"Troller %f hasar aldi. Guncel can degerleri %%%f\n",damage,troller_health/troller_health_max*100);
            }
                damage=0;
          }
                true_damage=0;
        }
    }

  if((round%2)==0)
    {
    //ork turu insan savunma
    if(dovuscucr!=0){
    if(round!=0&&round%(dovuscucr*2)==0&&gamei.units[4].kritik_sans!=0)
        ork_attack_power+=(float)(oseneryo.ork_fightern*gamei.units[4].saldiri*(150+okrit)/100);}
    if(mizrakcr){
    if(round!=0&&round%(mizrakcr*2)==0&&gamei.units[5].kritik_sans!=0)
        ork_attack_power+=(float)(oseneryo.ork_spearern*gamei.units[5].saldiri*(150+okrit)/100);}
    if(vargcr!=0){
    if(round!=0&&round%(vargcr*2)==0&&gamei.units[6].kritik_sans!=0)
        ork_attack_power+=(float)(oseneryo.varg_ridern*gamei.units[6].saldiri*(150+okrit)/100);}
    if(trollcr){
    if(round!=0&&round%(trollcr*2)==0&&gamei.units[7].kritik_sans!=0)
        ork_attack_power+=(float)(oseneryo.trolln*gamei.units[7].saldiri*(150+okrit)/100);}

    true_damage=ork_attack_power*(float)(1-human_defense_power/ork_attack_power);

        if(true_damage<=0)
            fprintf(log,"Tur bos gecildi.Ork irki hasar vuramadi.\n");
        else
        {
            damage=(float)((hseneryo.infantryn*gamei.units[0].savunma/human_defense_power)*true_damage);

            if(damage>0)
          {
              if (piyadeler_health<=damage) {
                hseneryo.infantryn = 0;
                piyadeler_health=0;
                fprintf(log,"Butun piyade birlikleri oldu.\n");
            } else {                piyadeler_health-=damage;
                fprintf(log,"Piyade birlikleri %f hasar aldi. Guncel can degerleri %%%f\n",damage,piyadeler_health/piyadeler_health_max*100);
            }

              damage=0;
          }
            damage=(float)((hseneryo.archern*gamei.units[1].savunma/human_defense_power)*true_damage);
            if(damage>0)
          {
              if (okcular_health<=damage) {
                hseneryo.archern = 0;
                suvariler_health=0;
                fprintf(log,"Butun okcu birlikleri oldu.\n");
            } else {                okcular_health-=damage;
                fprintf(log,"Okcu birlikleri %f hasar aldi. Guncel can degerleri %%%f\n",damage,okcular_health/okcular_health_max*100);

            }
              damage=0;

          }
            damage=(float)((hseneryo.cavalryn*gamei.units[2].savunma/human_defense_power)*true_damage);
            if(damage>0)
          {
              if (suvariler_health<=damage) {
                    fprintf(log,"Butun suvari birlikleri oldu.\n");
                    hseneryo.cavalryn = 0;
                    suvariler_health=0;
                } else {
                    suvariler_health-=damage;
                    fprintf(log,"Suvari birlikleri %f hasar aldi. Guncel can degerleri %%%f\n",damage,suvariler_health/suvariler_health_max*100);

                }
              damage=0;
          }
            damage=(float)((hseneryo.siege_machinen*gamei.units[3].savunma/human_defense_power)*true_damage);
            if(damage>0)
          {

              if (kusatma_makineleri_health<=damage) {
                    fprintf(log,"Butun kusatma makineleri imha edildi.\n");
                    hseneryo.siege_machinen = 0;
                    kusatma_makineleri_health=0;
                } else {                    kusatma_makineleri_health-=damage;
                    fprintf(log,"Kusatma makineleri %f hasar aldi. Guncel can degerleri %%%f\n",damage,kusatma_makineleri_health/kusatma_makineleri_health_max*100);
                }
              damage=0;
          }
            true_damage=0;
        }
    }

    infantry1=(int)hseneryo.infantryn+(int)hseneryo.archern+(int)hseneryo.cavalryn+(int)hseneryo.siege_machinen;
    infantry2=(int)oseneryo.ork_fightern+(int)oseneryo.ork_spearern+(int)oseneryo.varg_ridern+(int)oseneryo.trolln;

    if(infantry1<=0&&infantry2<=0){
       fprintf(log,"\n%d.turda: Insan irkindan hicbir birim kalmadi. Orc irkindan hicbir birim kalmadi. Berabere sonuclandi",round+1);}
    if(infantry1==0||infantry1<0){
        fprintf(log,"\n%d.turda: Insan irkindan hicbir birim kalmadi. Orc irki kazandi",round+1);}
    if(infantry2==0||infantry2<0){
        fprintf(log,"\n%d.turda: Ork irkindan hicbir birim kalmadi. Insan irki kazandi",round+1);}

  round++;

    fprintf(log,"\n");
}

soldiers[0].soldiersNum=hseneryo.infantryn;
soldiers[1].soldiersNum=hseneryo.archern;
soldiers[2].soldiersNum=hseneryo.cavalryn;
soldiers[3].soldiersNum=hseneryo.siege_machinen;
soldiers[4].soldiersNum=oseneryo.ork_fightern;
soldiers[5].soldiersNum=oseneryo.ork_spearern;
soldiers[6].soldiersNum=oseneryo.varg_ridern;
soldiers[7].soldiersNum=oseneryo.trolln;

initSDL("Savas Similasyonu");
    init();


    while(!game.loopDone)
    {
        update();
        updateInput();
        draw();
    }
    closeDownSDL();
    TTF_CloseFont(font);
    TTF_Quit();


return 0;
}

