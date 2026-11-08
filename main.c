#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void HELP();
//int counter_rows_and_col(FILE*, int*, long long*);
int target_row(const char*, long, char*);
int is_char_v_int(char*, int*, int);


int main(int argc, char* argv []){
    if (argc==1){
        fprintf(stderr, "No command\n");
        return 2;
    }
    else if(strcmp(argv[1], "--help")==0){
        //error
        if (argc>2){
            fprintf(stderr, "No command\n");
            return 2;
        }

        HELP();
    }
    else if(strcmp(argv[1], "schema")==0){
        //error
        if (argc!=3){
            fprintf(stderr, "Error: schema requires exactly one file\n");
            return 2;
        }
        
        FILE *f = fopen(argv[2], "r");
        if (f==NULL){
            fprintf(stderr, "file not found\n");
            return 1;
        }

        char line[4096];

        if(fgets(line, sizeof(line), f)==NULL){
            fprintf(stderr, "Empty file\n");
            fclose(f);
            return 1;
        }

        line[strcspn(line, "\n")]='\0';

        char *names[256];
        int col_count = 0;

        names[0]= line;
        col_count = 1;

        for(int i = 0; line[i] != '\0'; i++){
            if(line[i] == ','){
                line[i] = '\0';
                names[col_count] = &line[i + 1];
                col_count++;
            }
        }

        char rows[4096];
        long long row_count = 0;
        while (fgets(rows, sizeof(rows), f) != NULL){
            row_count++;
        }
        
        printf("rows %lld\n", row_count);
        printf("columns %d\n", col_count);
        for(int i = 0; i<col_count; i++){
            printf("%d \"%s\" INT\n", i, names[i]);
        }

        fclose(f);
    }
    else if(strcmp(argv[1], "stats")==0){
        //error 
        
        if (argc!=3){
            fprintf(stderr, "Error: stats requires exactly one file\n");
            return 2;
        }
        
        FILE *f = fopen(argv[2], "r");
        if (f==NULL){
            fprintf(stderr, "file not found");
            return 1;
        }

        /*
            column <i>	номер столбца (с 0)	i	%d
            name "<name>"	имя из заголовка	names[i]	\"%s\"
            type INT	тип (4/10 — всегда INT)	константа	INT
            nulls <n>	сколько NULL	nulls[i]	%lld
            min <v>	минимум (не-NULL)	min[i]	%lld
            max <v>	максимум (не-NULL)	max[i]	%lld
            mean <v>	среднее (не-NULL)	sum/(row-null)	%Lf (6 знаков)
            positive <n>	сколько > 0	positive[i]	%lld
            negative <n>	сколько < 0	negative[i]	%lld
            even <n>	сколько чётных	even[i]	%lld
        */
        
        char line[4096];

        if(fgets(line,sizeof(line),f)==NULL){
            fprintf(stderr, "Empty file");
            fclose(f);
            return 1;
        }

        char* names[256];
        int col_count=0;

        names[0]=line;
        col_count=1;

        for(int i=0; line[i]!='\0'; i++){
            if(line[i]==','){
                line[i]='\0';
                names[col_count]=&line[i+1];
                col_count++;
            }
            if(line[i]=='\n'){
                line[i]='\0';
            }
        }

        long long row_count = 0;
        char rows[4096];

        while(fgets(rows, sizeof(rows), f) != NULL){
            row_count++;
        }

        long long nulls = 0;
        long long min[256];
        long long max[256];
        long long sum[256];
        long long positiv[256] = {0};
        long long negativ[256] = {0};
        long long even[256];

        double mean[256];

        char str[4096];
        if(target_row(argv[2], 2, str)==1){
            fprintf(stderr, "out of range\n");
            return 1;
        }

        char* chisla[256];
        int col_ch_count=0;

        chisla[0] = str;
        col_ch_count++;

        for(int i=0; str[i]!='\0'; i++){
            if(str[i]==','){
                str[i]='\0';
                chisla[col_ch_count] = &str[i+1];
                col_ch_count++;    
            }
        }

        long long chislo;
        for(int i=0; i<col_ch_count; i++){
            chislo = strtoll(chisla[i], NULL, 10);
            min[i]=chislo;
            max[i]=chislo;
            sum[i]=chislo;
            if (chislo>0){
                positiv[i]=1;
            }
            else if (chislo<0){
                negativ[i]=1;
            }
            if(chislo%2==0){
                even[i]+=1;
            }
        }
        
        fclose(f);
        
        f=fopen(argv[2], "r");
        fgets( str, sizeof(str), f);
        fgets( str, sizeof(str), f);
    
        while(fgets( str, sizeof(str), f)!=NULL){
            str[strcspn(str,"\n")] = '\0';
            col_ch_count=0;
            chisla[0] = str;
            col_ch_count=1;
            for(int i=0; str[i]!='\0'; i++){
                if(str[i]==','){
                    str[i]='\0';
                    chisla[col_ch_count] = &str[i+1];
                    col_ch_count++;    
                }
            }
            
            for(int a=0; a<col_count;a++)
            {
                chislo = strtoll(chisla[a], NULL, 10);
                if(min[a]>chislo){
                    min[a]=chislo;
                }
                if(max[a]<chislo){
                    max[a]=chislo;
                }
                sum[a]+=chislo;
                if (chislo>0){
                    positiv[a]+=1;
                }
                else if (chislo<0){
                    negativ[a]+=1;
                }
                if(chislo%2==0){
                    even[a]+=1;
                }
            }
        }

        for(int i=0;i<col_count;i++){
            mean[i] = (double)sum[i] / (double)row_count;
        }

        printf("rows %d\n", row_count);
        printf("columns %lld\n", col_count);

        for(int i=0; i<col_count; i++){

        printf("\ncolumn %d\n", i);
        printf("name \"%s\"\n", names[i]);
        printf("type INT\n");
        printf("nulls %lld\n", nulls);
        printf("min %lld\n", min[i]);
        printf("max %lld\n", max[i]);
        printf("mean %f\n", mean[i]);
        printf("positive %lld\n", positiv[i]);
        printf("negative %lld\n", negativ[i]);
        printf("even %lld\n", even[i]);
        }

        fclose(f);
        
    /*
           if (argc != 3) {
        fprintf(stderr, "Error: stats requires exactly one file\n");
        return 2;
    }

    FILE *f = fopen(argv[2], "r");
    if (f == NULL) {
        fprintf(stderr, "Error: cannot open file '%s'\n", argv[2]);
        return 1;
    }

    // === 1. Заголовок ===
    char header[4096];
    if (fgets(header, sizeof(header), f) == NULL) {
        fprintf(stderr, "Error: empty file\n");
        fclose(f);
        return 1;
    }
    header[strcspn(header, "\n")] = '\0';

    char *names[256];
    int col_count = 0;
    names[0] = header;
    col_count = 1;
    for (int i = 0; header[i] != '\0'; i++) {
        if (header[i] == ',') {
            header[i] = '\0';
            names[col_count] = &header[i + 1];
            col_count++;
        }
    }

    // === 2. Инициализация статистики ===
    long long min[256], max[256], positive[256], negative[256], even[256], nulls[256];
    long double sum[256];

    for (int i = 0; i < col_count; i++) {
        min[i] = LLONG_MAX;      // самое большое — первое значение станет min
        max[i] = LLONG_MIN;      // самое маленькое — первое значение станет max
        sum[i] = 0.0L;           // long double
        positive[i] = 0;
        negative[i] = 0;
        even[i] = 0;
        nulls[i] = 0;
    }

    // === 3. Один проход по строкам данных ===
    long long row_count = 0;
    char line[4096];

    while (fgets(line, sizeof(line), f) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        row_count++;

        // Разбить line на поля
        char *fields[256];
        int field_count = 0;
        fields[0] = line;
        field_count = 1;
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == ',') {
                line[i] = '\0';
                fields[field_count] = &line[i + 1];
                field_count++;
            }
        }

        // Обновить статистику по каждому столбцу
        for (int a = 0; a < col_count; a++) {
            long long val = strtoll(fields[a], NULL, 10);
            if (val < min[a]) min[a] = val;
            if (val > max[a]) max[a] = val;
            sum[a] += (long double)val;
            if (val > 0) positive[a]++;
            else if (val < 0) negative[a]++;
            if (val % 2 == 0) even[a]++;
        }
    }

    fclose(f);

    // === 4. Вывод ===
    for (int i = 0; i < col_count; i++) {
        long long non_null = row_count - nulls[i];
        long double mean;
        if (non_null > 0) {
            mean = sum[i] / (long double)non_null;
        } else {
            mean = 0.0L;
        }
        if (mean == 0.0L) mean = 0.0L;   // убрать -0.0

        printf("column %d\n", i);
        printf("name \"%s\"\n", names[i]);
        printf("type INT\n");
        printf("nulls %lld\n", nulls[i]);
        printf("min %lld\n", min[i]);
        printf("max %lld\n", max[i]);
        printf("mean %Le\n", mean);
        printf("positive %lld\n", positive[i]);
        printf("negative %lld\n", negative[i]);
        printf("even %lld\n", even[i]);

        if (i < col_count - 1) printf("\n");   // пустая строка между блоками
    }*/ 
    }
    else{
        fprintf(stderr, "No command\n");
        return 2;
    }

    return 0;
}

void HELP(){
    puts("Usage: csvstat <command> <file> [options]");
    puts("Commands:");
    puts(" --help           - print list of commands");
    puts(" schema <file>    - print table schema (rows, columns, types)");
    puts(" stat <file>      - print column statistics");
}

/*
int counter_row_and_col(FILE* f, int* col_count, long long* row_count){
    char line[4096];

    if(fgets(,sizeof(line),f))
}*/

int target_row(const char* text_file, long target, char* final) {
    FILE* f = fopen(text_file,"r");
    if(f==NULL){
        return 1;
    }

    char str[4096];
    for(int i=0;i<target;i++){
        if(fgets( str, sizeof(str), f)==NULL){
            fclose(f);
            return 1;
        }
        str[strcspn(str,"\n")] = '\0';
    }

    strcpy(final, str);

    fclose(f);

    return 0;
}
/*
int is_char_v_int(char* str, int* chislo, int colvo){
    for(int i=0; i<colvo; i++){
        chislo = atoi(str);
    }
}*/
