#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "SENNA_utils.h"
#include "SENNA_Hash.h"
#include "SENNA_Tokenizer.h"

#include "SENNA_POS.h"
#include "SENNA_CHK.h"
#include "SENNA_NER.h"
#include "SENNA_VBS.h"
#include "SENNA_SRL.h"
#include "SENNA_PT0.h"

/* fgets max sizes */
#define MAX_SENTENCE_SIZE 1024
#define MAX_TARGET_VB_SIZE 256

int main(int argc, char *argv[])
{
  int i, j,prev;
  char* buf;

  /* options */
  char *opt_path = "/var/www/hack/";
  int opt_verbose = 0;
  int opt_notokentags = 0;
  int opt_iobtags = 0;
  int opt_brackettags = 0;
  int opt_posvbs = 0;
  int opt_usrtokens = 0;
  int opt_pos = 1;
  int opt_chk = 0;
  int opt_ner = 0;
  int opt_srl = 0;
  FILE *opt_usrvbs = NULL;

  SENNA_set_verbose_mode(opt_verbose);

  if(!opt_pos && !opt_chk && !opt_ner && !opt_srl) /* the user does not know what he wants... */
    opt_pos = opt_chk = opt_ner = opt_srl = 1;     /* so give him everything */

  char * sentence = argv[1];

  /* the real thing */
  {
    //char sentence[MAX_SENTENCE_SIZE];
    char target_vb[MAX_TARGET_VB_SIZE];
    int *chk_labels = NULL;
    int *pt0_labels = NULL;
    int *pos_labels = NULL;
    int *ner_labels = NULL;
    int *vbs_labels = NULL;
    int **srl_labels = NULL;
    int vbs_hash_novb_idx = 22;
    int n_verbs = 0;

    /* inputs */
    SENNA_Hash *word_hash = SENNA_Hash_new(opt_path, "hash/words.lst");
    SENNA_Hash *caps_hash = SENNA_Hash_new(opt_path, "hash/caps.lst");
    SENNA_Hash *suff_hash = SENNA_Hash_new(opt_path, "hash/suffix.lst");
    SENNA_Hash *gazt_hash = SENNA_Hash_new(opt_path, "hash/gazetteer.lst");

    SENNA_Hash *gazl_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.loc.lst", "data/ner.loc.dat");
    SENNA_Hash *gazm_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.msc.lst", "data/ner.msc.dat");
    SENNA_Hash *gazo_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.org.lst", "data/ner.org.dat");
    SENNA_Hash *gazp_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.per.lst", "data/ner.per.dat");

    /* labels */
    SENNA_Hash *pos_hash = SENNA_Hash_new(opt_path, "hash/pos.lst");
    SENNA_Hash *chk_hash = SENNA_Hash_new(opt_path, "hash/chk.lst");
    SENNA_Hash *pt0_hash = SENNA_Hash_new(opt_path, "hash/pt0.lst");
    SENNA_Hash *ner_hash = SENNA_Hash_new(opt_path, "hash/ner.lst");
    SENNA_Hash *vbs_hash = SENNA_Hash_new(opt_path, "hash/vbs.lst");
    SENNA_Hash *srl_hash = SENNA_Hash_new(opt_path, "hash/srl.lst");

    SENNA_POS *pos = SENNA_POS_new(opt_path, "data/pos.dat");
    SENNA_CHK *chk = SENNA_CHK_new(opt_path, "data/chk.dat");
    SENNA_PT0 *pt0 = SENNA_PT0_new(opt_path, "data/pt0.dat");
    SENNA_NER *ner = SENNA_NER_new(opt_path, "data/ner.dat");
    SENNA_VBS *vbs = SENNA_VBS_new(opt_path, "data/vbs.dat");
    SENNA_SRL *srl = SENNA_SRL_new(opt_path, "data/srl.dat");

    SENNA_Tokenizer *tokenizer = SENNA_Tokenizer_new(word_hash, caps_hash, suff_hash, gazt_hash, gazl_hash, gazm_hash, gazo_hash, gazp_hash, opt_usrtokens);


    //while(fgets(sentence, MAX_SENTENCE_SIZE, stdin))
    if(sentence != NULL)
    {
      SENNA_Tokens* tokens = SENNA_Tokenizer_tokenize(tokenizer, sentence);

      //if(tokens->n == 0)
        //continue;
      //printf("ok\n");

      pos_labels = SENNA_POS_forward(pos, tokens->word_idx, tokens->caps_idx, tokens->suff_idx, tokens->n);
      if(opt_chk)
        chk_labels = SENNA_CHK_forward(chk, tokens->word_idx, tokens->caps_idx, pos_labels, tokens->n);
      if(opt_srl)
        pt0_labels = SENNA_PT0_forward(pt0, tokens->word_idx, tokens->caps_idx, pos_labels, tokens->n);
      if(opt_ner)
        ner_labels = SENNA_NER_forward(ner, tokens->word_idx, tokens->caps_idx, tokens->gazl_idx, tokens->gazm_idx, tokens->gazo_idx, tokens->gazp_idx, tokens->n);
      if(opt_srl)
      {
        if(opt_usrvbs)
        {
          vbs_labels = SENNA_realloc(vbs_labels, sizeof(int), tokens->n);
          n_verbs = 0;
          for(i = 0; i < tokens->n; i++)
          {
            if(!SENNA_fgetline(target_vb, MAX_TARGET_VB_SIZE, opt_usrvbs))
              SENNA_error("invalid user verbs file\n");
            vbs_labels[i] = !( (target_vb[0] == '-') && ( (target_vb[1] == '\0') || isspace(target_vb[1])) );
            n_verbs += vbs_labels[i];
          }
          if(!SENNA_fgetline(target_vb, MAX_TARGET_VB_SIZE, opt_usrvbs))
            SENNA_error("invalid user verbs file\n");
          if(strlen(target_vb) > 0)
            SENNA_error("sentence size does not match in user verbs file");
        }
        else if(opt_posvbs)
        {
          vbs_labels = SENNA_realloc(vbs_labels, sizeof(int), tokens->n);
          n_verbs = 0;
          for(i = 0; i < tokens->n; i++)
          {
            vbs_labels[i] = (SENNA_Hash_key(pos_hash, pos_labels[i])[0] == 'V');
            n_verbs += vbs_labels[i];
          }
        }
        else
        {
          vbs_labels = SENNA_VBS_forward(vbs, tokens->word_idx, tokens->caps_idx, pos_labels, tokens->n);
          n_verbs = 0;
          for(i = 0; i < tokens->n; i++)
          {
            vbs_labels[i] = (vbs_labels[i] != vbs_hash_novb_idx);
            n_verbs += vbs_labels[i];
          }
        }
      }

      if(opt_srl)
        srl_labels = SENNA_SRL_forward(srl, tokens->word_idx, tokens->caps_idx, pt0_labels, vbs_labels, tokens->n);

      prev=0;
      for(i = 0; i < tokens->n; i++)
      {
          //printf("%15s", tokens->words[i]);
          //printf("\t%10s", SENNA_Hash_key(pos_hash, pos_labels[i]));
          buf=(char*)SENNA_Hash_key(pos_hash, pos_labels[i]);
          if(strlen(buf)>=2 && buf[0]=='N' && buf[1]=='N'){
              printf("%s ",tokens->words[i]);
              prev=1;
          }
          else if(prev==1){
              prev=0;
              printf(" ");
          }
          else prev=0;
      }
      printf("\n");
    }

    if(opt_posvbs)
      SENNA_free(vbs_labels);

    if(opt_usrvbs)
    {
      SENNA_free(vbs_labels);
      SENNA_fclose(opt_usrvbs);
    }

    SENNA_Tokenizer_free(tokenizer);

    SENNA_POS_free(pos);
    SENNA_CHK_free(chk);
    SENNA_PT0_free(pt0);
    SENNA_NER_free(ner);
    SENNA_VBS_free(vbs);
    SENNA_SRL_free(srl);

    SENNA_Hash_free(word_hash);
    SENNA_Hash_free(caps_hash);
    SENNA_Hash_free(suff_hash);
    SENNA_Hash_free(gazt_hash);

    SENNA_Hash_free(gazl_hash);
    SENNA_Hash_free(gazm_hash);
    SENNA_Hash_free(gazo_hash);
    SENNA_Hash_free(gazp_hash);

    SENNA_Hash_free(pos_hash);
    SENNA_Hash_free(chk_hash);
    SENNA_Hash_free(pt0_hash);
    SENNA_Hash_free(ner_hash);
    SENNA_Hash_free(vbs_hash);
    SENNA_Hash_free(srl_hash);
  }

  return 0;
}
