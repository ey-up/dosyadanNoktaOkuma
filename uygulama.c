#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <conio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct noktaTutar{
	float x,y,z;
	int r,g,b;
};

struct dosyalar {
  char dosyaAdlari[50];
  char ciktiAdlari[50];
  int noktaSayisi;
  int veriTipi;
  int hataliMi;
  double uzaklikOrt;
  int rgbliMi;
};

FILE *ac;
FILE *cik;
int dosyaSayisi;
struct dosyalar *dosyalarim;
float enUzakNokta[6];
float enYakinNokta[6];
int eh=0,he=0;

void menu();
void dosyaKontrol(struct dosyalar *dos);
void uzaklikYakinlik(struct dosyalar *dos);
void kupHesapla(struct dosyalar *dos);
void kureHesapla(struct dosyalar *dos);
void noktaUzakOrt(struct dosyalar *dos);


int main(int argc, char const *argv[]) {
	mkdir("ciktilar");
	struct dirent *dosya;
	int i=0;
	DIR *p;
	p = opendir(".");
	dosyalarim = malloc ( 1 * sizeof(struct dosyalar));
	while ((dosya = readdir(p)) != NULL ){
		int length = strlen(dosya->d_name);
		if (strncmp(dosya->d_name + length - 4, ".nkt", 4) == 0){
			dosyalarim = realloc(dosyalarim, (i+1) * sizeof(struct dosyalar));
		    strcpy(dosyalarim[i].dosyaAdlari,dosya->d_name);
		    strcpy(dosyalarim[i].ciktiAdlari,"ciktilar/sonuc");
				strcat(dosyalarim[i].ciktiAdlari,dosya->d_name);
				dosyalarim[i].noktaSayisi=0;
				dosyalarim[i].hataliMi=1;
				dosyalarim[i].veriTipi=0;
		  	dosyalarim[i].uzaklikOrt=0;
				dosyalarim[i].rgbliMi=0;
				i++;
		}
	}
	dosyaSayisi=i;
    (void) closedir(p);
	menu();
  	return 0;
}
void menu() {
	int islem=0;
  	git:
  	printf("\n\n1) Dosya Kontrolu\n");
  	printf("2) En Yakin/Uzak Noktala\n");
  	printf("3) Kup\n");
  	printf("4) Kure\n");
  	printf("5) Nokta Uzakliklari\n");
  	printf("6) Cikis \n");
  	printf("Secim yapiniz->");
  	scanf("%d",&islem);
  	switch(islem)
  	{
    	case 1:
      	{
        	dosyaKontrol(dosyalarim);
          	goto git;
      	}
      	case 2:
      	{
					if(eh==1 || eh==2){
					uzaklikYakinlik(dosyalarim);
					}
					else printf("\n1. etaba ugramaniz gerekmektedir.\n");
        	goto git;
      	}
      	case 3:
      	{
					if(eh==1 || eh==2){
						kupHesapla(dosyalarim);
					}
					else printf("\n 1. etaba ugramaniz gerekmektedir.\n");
        	goto git;
      	}
      	case 4:
      	{
						if(eh==1 || eh==2){
							kureHesapla(dosyalarim);
						}
						else printf("\n 1. etaba ugramaniz gerekmektedir.\n");
          	goto git;
      	}
      	case 5:
      	{
					if(he==2){
						noktaUzakOrt(dosyalarim);
					}else printf("\n1. ve 2. adimlara ugramaniz gerekmektedir.\n");
					goto git;
      	}
				case 6:{
						exit(1);
				}
      	default:
      	{   printf("Yanlis secim \n");
        	goto git;
     	}

	}
}
void dosyaKontrol(struct dosyalar *dos){

	char cumle[100],bosluk[15];
	int i=0,satir=0,boyut,boslk;
	while(( ac = fopen (dos[i].dosyaAdlari,"r" ) ) !=NULL ){
		satir=0;
		cik = fopen(dos[i].ciktiAdlari,"a");
		fputs("SECIM1\n",cik);
		while(fgets(cumle,100,ac) !=NULL){

			if (satir == 4){
				char sil[10],veriTip[10];
				sscanf(cumle,"%s %s",&sil,&veriTip);
				if(  strcmp(veriTip,"binary") ==0 ){
					dos[i].veriTipi=1;
					}
				}

			if(satir==4 && dos[i].veriTipi==1){
				int u=0;
				if( dos[i].rgbliMi==1){
					struct noktaTutar s;
					while (1) {
						size_t sayi = fread(&s, sizeof(struct noktaTutar), 1, ac);
						if(sayi<1) break;
							//printf("%f %f %f %d %d %d\n",s.x,s.y,s.z ,s.r,s.g,s.b);
						if(s.r>255 || s.g>255 || s.b>255 ){
							fprintf(cik,"%d. satirda rgb bilgisi uygun degildir.\n",u+5);
							goto gg;
						}
						u++;
					}
				}
				if(dos[i].rgbliMi==0){
					struct noktaTutar s;
					while (1) {
						size_t sayi = fread(&s, sizeof(struct noktaTutar), 1, ac);
						if(sayi<1) break;
							// printf("%f %f %f \n",s.x,s.y,s.z );
							u++;
					}
				}
				if(u != dos[i].noktaSayisi){
					dos[i].hataliMi=0;
					fprintf(cik,"%d nokta sayisi %d olmaliydi\n",u,dos[i].noktaSayisi);
					goto gg;
				}
					goto gg;
			}

			if(satir>4 && dos[i].veriTipi==0) {
				int r,g,b1,j=0;
				float x1,x2,x3;
				boslk=0;
				while(cumle[j] != '\n'){
					if(cumle[j]==' ')
						boslk++;
					j++;
				}
				if(dos[i].rgbliMi==0){
					if(boslk != 2){
						dos[i].hataliMi=0;
						fprintf(cik,"%d. satir bilgileri yanlistir.\n",satir+1);
						goto gg;
					}
				}
				else if(dos[i].rgbliMi==1){
					if(boslk != 5){
						dos[i].hataliMi=0;
						fprintf(cik,"%d. satir bilgileri yanlistir.\n",satir+1);
						goto gg;
					}
					sscanf(cumle,"%f %f %f %d %d %d",&x1,&x2,&x3,&r,&g,&b1);
					if( r>255 || g>255 || b1>255 ){
						fprintf(cik,"%d. satirda rgb bilgisi uygun degildir.\n",satir+1);
						dos[i].hataliMi=0;
						goto gg;
					}
				}
			}

			if(satir==3){
				sscanf(cumle,"%s %d",&bosluk, &dos[i].noktaSayisi);
			}

			if(satir==2){
				char c[15]="ALANLAR x y z";
				char c1[20]="ALANLAR x y z r g b";
				int j=0;
				boyut=strlen(cumle);
				if(boyut==14){
					dos[i].rgbliMi=0;
					for(j=0; j<boyut-1; j++){
						if(cumle[j]!= c[j]){
                        	dos[i].hataliMi=0;
                		}
					}
				}
				else if(boyut==20){
					dos[i].rgbliMi=1;
					for(j=0; j<boyut-1; j++){
						if(cumle[j]!= c1[j]){
                        	dos[i].hataliMi=0;
                       	}
					}
				}else{
					dos[i].hataliMi=0;
				}
				if(dos[i].hataliMi==0){
					fputs("ALANLAR istenilen formatta degildir. \n",cik);
					goto gg;
					}
			}

			if(satir==1){
				int k;
				char t[100];
				sscanf(cumle,"%s %d",&t,&k);
				if(   !(  (strcmp(t,"VERSION") ==0)   && (k==1)) ){
					fputs("Version bilgisi yanlistir. \n",cik);
					dos[i].hataliMi=0;
				}
			}
			tt:
			satir++;
		}
		if(satir-5 != dos[i].noktaSayisi){
			dos[i].hataliMi=0;
			fprintf(cik,"%d nokta sayisi %d olmaliydi\n",satir-5,dos[i].noktaSayisi);
		}
		gg:
		if(dos[i].hataliMi==1){
			fputs("Dosya uyumludur \n",cik);
		}
		fclose(cik);
		fclose(ac);
		i++;
	}
	eh=1;
}
void uzaklikYakinlik (struct dosyalar *dos) {
	eh=2; he=2;
	char cumle[100];
	int i=0,satir=0;
	float deger=0;
	while(( ac = fopen (dos[i].dosyaAdlari,"r" ) ) !=NULL ){
	  	 satir=0;
		 cik = fopen(dos[i].ciktiAdlari,"a");
	  	 if(dos[i].hataliMi==1){
	      	fputs("SECIM2\n",cik);
	      	float noktalar[dos[i].noktaSayisi][6];
        	while(fgets(cumle,100,ac) != NULL){
				if(satir==4 && dos[i].veriTipi==1){
					int h=0;
					struct noktaTutar s;
					while(1){
						size_t sayi = fread(&s, sizeof(struct noktaTutar), 1,ac);
						if(sayi < 1) break;
						noktalar[h][0]=s.x;
						noktalar[h][1]=s.y;
						noktalar[h][2]=s.z;
						h++;
					}
				}
		   	    if(satir>4 && dos[i].veriTipi==0){
		   			   sscanf(cumle,"%f %f %f",&noktalar[satir-5][0], &noktalar[satir-5][1], &noktalar[satir-5][2]);
				}
			   	satir++;
			}

			int j=0,t;
			float enyakin=0,temp=0,enuzak=0;
      		int s1,s2,s3,s4;
			enyakin=sqrt(  pow(( noktalar[0][0] - noktalar[1][0]),2)  +  pow(( noktalar[0][1] - noktalar[1][1]),2)   +   pow(( noktalar[0][2] - noktalar[1][2]),2)  );
			enuzak=sqrt(  pow(( noktalar[0][0] - noktalar[1][0]),2)  +  pow(( noktalar[0][1] - noktalar[1][1]),2)   +   pow(( noktalar[0][2] - noktalar[1][2]),2)  );
		 	for(j=0; j<dos[i].noktaSayisi-1; j++ ){
		 		for(t=j+1; t<dos[i].noktaSayisi; t++){
		 			deger=sqrt(  pow(( noktalar[j][0] - noktalar[t][0]),2)   +  pow(( noktalar[j][1] - noktalar[t][1]),2)   +   pow(( noktalar[j][2] - noktalar[t][2]),2)  );
					dos[i].uzaklikOrt = dos[i].uzaklikOrt + deger;
			 		if(deger<enyakin){
           				s1=j;
            			s2=t;
			 			enyakin=deger;
			 			enYakinNokta[0]=noktalar[j][0];
			 			enYakinNokta[1]=noktalar[j][1];
			 			enYakinNokta[2]=noktalar[j][2];
			 			enYakinNokta[3]=noktalar[t][0];
			 			enYakinNokta[4]=noktalar[t][1];
			 			enYakinNokta[5]=noktalar[t][2];
				 	}
					if(deger>enuzak){
            			s3=j;
            			s4=t;
			 			enuzak=deger;
			 			enUzakNokta[0]=noktalar[j][0];
			 			enUzakNokta[1]=noktalar[j][1];
			 			enUzakNokta[2]=noktalar[j][2];
			 			enUzakNokta[3]=noktalar[t][0];
			 			enUzakNokta[4]=noktalar[t][1];
			 			enUzakNokta[5]=noktalar[t][2];
					}
				}
			}
			dos[i].uzaklikOrt = dos[i].uzaklikOrt / (   (double)dos[i].noktaSayisi * ( (double)dos[i].noktaSayisi-1 )   / 2.0    );
			if(dos[i].veriTipi==0){
				fprintf(cik,"%d. satir -> %f  %f  %f\n",s3+6,enUzakNokta[0],enUzakNokta[1],enUzakNokta[2]);
				fprintf(cik,"%d. satir -> %f  %f  %f\n",s4+6,enUzakNokta[3],enUzakNokta[4],enUzakNokta[5]);
				fprintf(cik,"%d. satir -> %f  %f  %f\n",s1+6,enYakinNokta[0],enYakinNokta[1],enYakinNokta[2]);
				fprintf(cik,"%d. satir -> %f  %f  %f\n",s2+6,enYakinNokta[3],enYakinNokta[4],enYakinNokta[5]);
			}else if(dos[i].veriTipi==1){
				fwrite(&enYakinNokta[0], sizeof(float),1,cik);
				fwrite(&enYakinNokta[1], sizeof(float),1,cik);
				fwrite(&enYakinNokta[2], sizeof(float),1,cik);
				fputs("\n",cik);
				fwrite(&enYakinNokta[3], sizeof(float),1,cik);
				fwrite(&enYakinNokta[4], sizeof(float),1,cik);
				fwrite(&enYakinNokta[5], sizeof(float),1,cik);
				fputs("\n",cik);
				fwrite(&enUzakNokta[0], sizeof(float),1,cik);
				fwrite(&enUzakNokta[1], sizeof(float),1,cik);
				fwrite(&enUzakNokta[2], sizeof(float),1,cik);
				fputs("\n",cik);
				fwrite(&enUzakNokta[3], sizeof(float),1,cik);
				fwrite(&enUzakNokta[4], sizeof(float),1,cik);
				fwrite(&enUzakNokta[5], sizeof(float),1,cik);
				fputs("\n",cik);
			}
		}
		fclose(cik);
		fclose(ac);
		i++;
	}
}
void kupHesapla(struct dosyalar *dos) {
  	char cumle[100];
	int i=0,satir=0;

	while(( ac = fopen (dos[i].dosyaAdlari,"r" ) ) !=NULL ){
    	satir=0;
    	cik = fopen(dos[i].ciktiAdlari,"a");
    	if(dos[i].hataliMi==1){
      		fputs("SECIM3\n",cik);
      		float noktalar[dos[i].noktaSayisi][6];
      		while(fgets(cumle,100,ac) != NULL){
				if(satir==4 && dos[i].veriTipi==1){
					int h=0;
					struct noktaTutar s;
					while(1){
						size_t sayi = fread(&s, sizeof(struct noktaTutar), 1,ac);
						if(sayi < 1) break;
						noktalar[h][0]=s.x;
						noktalar[h][1]=s.y;
						noktalar[h][2]=s.z;
						h++;
					}
				}
        		if(satir>4 && dos[i].veriTipi==0){
          			sscanf(cumle,"%f %f %f",&noktalar[satir-5][0], &noktalar[satir-5][1], &noktalar[satir-5][2]);
           		}
          	satir++;
      		}
      		int j,t;
      		float enKucuk=0,enBuyuk=0;
      		enKucuk=noktalar[0][0];
      		for(j=0; j<dos[i].noktaSayisi; j++){
      			for(t=0; t<3; t++){
      				if(noktalar[j][t] > enBuyuk ){
						enBuyuk=noktalar[j][t];
			  		}
		  			if(noktalar[j][t] < enKucuk){
		  				enKucuk = noktalar[j][t];
			  		}
		  		}
	  		}
	  		if(dos[i].veriTipi==0){
	  			fprintf(cik,"%f  %f  %f\n",enBuyuk,enBuyuk,enBuyuk);
		  		fprintf(cik,"%f  %f  %f\n",enBuyuk,enBuyuk,enKucuk);
				fprintf(cik,"%f  %f  %f\n",enBuyuk,enKucuk,enBuyuk);
				fprintf(cik,"%f  %f  %f\n",enBuyuk,enKucuk,enKucuk);
				fprintf(cik,"%f  %f  %f\n",enKucuk,enKucuk,enKucuk);
				fprintf(cik,"%f  %f  %f\n",enKucuk,enKucuk,enBuyuk);
			  	fprintf(cik,"%f  %f  %f\n",enKucuk,enBuyuk,enKucuk);
			  	fprintf(cik,"%f  %f  %f\n",enKucuk,enBuyuk,enBuyuk);
		  	}else if(dos[i].veriTipi==1){
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fputs("\n",cik);
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fwrite(&enKucuk, sizeof(float),1,cik);
			  	fputs("\n",cik);
				fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fwrite(&enKucuk, sizeof(float),1,cik);
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fputs("\n",cik);
				fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fwrite(&enKucuk, sizeof(float),1,cik);
			  	fwrite(&enKucuk, sizeof(float),1,cik);
			  	fputs("\n",cik);
				fwrite(&enKucuk, sizeof(float),1,cik);
			  	fwrite(&enKucuk, sizeof(float),1,cik);
			  	fwrite(&enKucuk, sizeof(float),1,cik);
			  	fputs("\n",cik);
				fwrite(&enKucuk, sizeof(float),1,cik);
			  	fwrite(&enKucuk, sizeof(float),1,cik);
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fputs("\n",cik);
				fwrite(&enKucuk, sizeof(float),1,cik);
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fwrite(&enKucuk, sizeof(float),1,cik);
			  	fputs("\n",cik);
				fwrite(&enKucuk, sizeof(float),1,cik);
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fwrite(&enBuyuk, sizeof(float),1,cik);
			  	fputs("\n",cik);
		  	}
    	}
    	fclose(cik);
    	fclose(ac);
    	i++;
  	}
}
void kureHesapla(struct dosyalar *dos){
	float cx,cy,cz,cr;
	printf(" X degerini giriniz:"); scanf("%f",&cx);
	printf(" Y degerini giriniz:"); scanf("%f",&cy);
	printf(" Z degerini giriniz:"); scanf("%f",&cz);
	printf(" R degerini giriniz:"); scanf("%f",&cr);
	int i=0,satir=0;
	char cumle[100];
	while(( ac = fopen (dos[i].dosyaAdlari,"r" ) ) !=NULL ){
		satir=0;
	  	cik = fopen(dos[i].ciktiAdlari,"a");
	    if(dos[i].hataliMi==1){
	    	fputs("SECIM4\n",cik);
	    	float noktalar[dos[i].noktaSayisi][6];
	        while(fgets(cumle,100,ac) != NULL){
	      		if(satir==4 && dos[i].veriTipi==1){
	        		int h=0;
	        		struct noktaTutar s;
	        		while(1){
	        			size_t sayi = fread(&s, sizeof(struct noktaTutar), 1,ac);
	        			if(sayi < 1) break;
	          			noktalar[h][0]=s.x;
				        noktalar[h][1]=s.y;
				        noktalar[h][2]=s.z;
	          			h++;
	        		}
	      		}
	            if(satir>4 && dos[i].veriTipi==0){
	                sscanf(cumle,"%f %f %f",&noktalar[satir-5][0], &noktalar[satir-5][1], &noktalar[satir-5][2]);
	            }
	            satir++;
	        }
	        	int j;
	        	float sonuc=0;
	        for(j=0; j<dos[i].noktaSayisi; j++){
	        	sonuc = sqrt( pow(cx-noktalar[j][0],2) + pow(cy-noktalar[j][1],2) + pow(cz-noktalar[j][2],2) );
	          	if(sonuc <= cr ){
	          		if(dos[i].veriTipi==0){
	            		fprintf(cik,"%d. satir %f %f %f\n",j+6, noktalar[j][0], noktalar[j][1], noktalar[j][2]);
	          		}
	          		if(dos[i].veriTipi==1){
						fprintf(cik,"%d. satir -> ",j+6);
			            fwrite(&noktalar[j][0], sizeof(float),1,cik);
			            fwrite(&noktalar[j][1], sizeof(float),1,cik);
			            fwrite(&noktalar[j][2], sizeof(float),1,cik);
			            fputs("\n",cik);
		          	}
	    		}
	      	}
	    }
	  	fclose(cik);
	    fclose(ac);
	    i++;
	}
}
void noktaUzakOrt(struct dosyalar *dos) {
	int i;
	for(i=0; i<dosyaSayisi; i++){
		if(dos[i].hataliMi==1){
			cik = fopen(dos[i].ciktiAdlari,"a");
			fputs("SECIM5\n",cik);
			if(dos[i].veriTipi==0)
				fprintf(cik,"%f ",dos[i].uzaklikOrt);
			else
				fwrite(&dos[i].uzaklikOrt, sizeof(double),1,cik);
		}
		fclose(cik);
	}
}
