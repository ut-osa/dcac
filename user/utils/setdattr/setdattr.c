#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../../include/dcac.h"

#define die(fmt, args...) do { fprintf(stderr, fmt, ##args); exit(1); } while(0)

struct options {
   char *filename;
   int type;
   char *acl;
   char *op;
} options;

void usage(char *prog_name)
{
   printf("Usage: %s <filename> <mode=acl>\n", prog_name);
}

int parse_acl_op(struct options *options)
{
   int type = -1;
   char *op = options->op;

   if (strlen(op) < 2)
      return -1;

   if (op[1] != '=')
      return -1;

   switch (op[0]) {
   case 'r':
      type = DCAC_OP_SET_FILE_RDACL;
      break;
   case 'w':
      type = DCAC_OP_SET_FILE_WRACL;
      break;
   case 'x':
      type = DCAC_OP_SET_FILE_EXACL;
      break;
   case 'm':
      type = DCAC_OP_SET_FILE_MDACL;
      break;
   }

   if (type == -1)
      return -1;

   options->type = type;
   options->acl = options->op + 2;

   return 0;
}

int parse_args(int argc, char *argv[])
{
   if (argc != 3)
      return -1;

   options.filename = argv[1];
   options.op = argv[2];

   if (parse_acl_op(&options) < 0)
      return -1;

   return 0;
}

int main(int argc, char *argv[])
{
   int rc;

   if (parse_args(argc, argv) < 0) {
      usage(argv[0]);
      exit(1);
   }

   switch (options.type) {
   case DCAC_OP_SET_FILE_RDACL:
      rc = dcac_set_file_rdacl(options.filename, options.acl);
      break;
   case DCAC_OP_SET_FILE_WRACL:
      rc = dcac_set_file_wracl(options.filename, options.acl);
      break;
   case DCAC_OP_SET_FILE_EXACL:
      rc = dcac_set_file_exacl(options.filename, options.acl);
      break;
   case DCAC_OP_SET_FILE_MDACL:
      rc = dcac_set_file_mdacl(options.filename, options.acl);
      break;
   default:
      fprintf(stderr, "Unexpected unknown peration\n");
      exit(1);
   }

   if (rc < 0) {
      fprintf(stderr, "dcac_set_file: %s\n", strerror(errno));
      exit(1);
   }

   return 0;
}
