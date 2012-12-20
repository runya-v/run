// addr2line


#include <cstdlib>
#include <iostream>


#include <bfd.h>
#include <getopt.h>
#include <libiberty.h>
#include <demangle.h>
//#include <string.h>
//#include <bucomm.h>
//#include <budemang.h>

static bfd_boolean with_functions; // -f, show function names.
static bfd_boolean do_demangle;    // -C, demangle names.
static bfd_boolean base_names;     // -s, strip directory names.

static int naddr;    // Number of addresses to process.
static char **addr;  // Hex addresses to process.

static asymbol **syms;  // Symbol table.

static struct option long_options[] =
{
  {"basenames", no_argument, 0, 's'},
  {"demangle",  optional_argument, 0, 'C'},
  {"exe",       required_argument, 0, 'e'},
  {"functions", no_argument, 0, 'f'},
  {"target",    required_argument, 0, 'b'},
  {"help",      no_argument, 0, 'H'},
  {"version",   no_argument, 0, 'V'},
  {0,           no_argument, 0, 0}
}
;

static void usage(FILE *, int);
static void slurp_symtab(bfd *);
static void find_address_in_section(bfd *, asection *, PTR);
static void translate_addresses(bfd *);
static void process_file(const char *, const char *);

char *program_name;


#define TARGET "i686-pc-linux-gnu"

void set_default_bfd_target ()
{
  /* The macro TARGET is defined by Makefile.  */
  const char *target = TARGET;

  if (! bfd_set_default_target (target))
    std::cerr
			<< "can't set BFD default target to `%s': %s"
			<< target << bfd_errmsg(bfd_get_error()) << std::endl;
}

void bfd_nonfatal(const char *string)
{
  const char *errmsg = bfd_errmsg (bfd_get_error ());

  if (string)
    fprintf (stderr, "%s: %s: %s\n", program_name, string, errmsg);
  else
    fprintf (stderr, "%s: %s\n", program_name, errmsg);
}


void bfd_fatal(const char *string)
{
  bfd_nonfatal (string);
  xexit (1);
}

/* Print a usage message to STREAM and exit with STATUS.  */
static void usage(FILE *stream, int status)
{}


/* Read in the symbol table.  */
static void slurp_symtab(bfd *abfd)
{
  long symcount;
  unsigned int size;

  if ((bfd_get_file_flags(abfd) & HAS_SYMS) == 0)
    return;

  symcount = bfd_read_minisymbols(abfd, false, (PTR)&syms, &size);

	if (symcount == 0)
    symcount = bfd_read_minisymbols(abfd, true, (PTR) &syms, &size);

  if (symcount < 0)
    bfd_fatal(bfd_get_filename(abfd));
}




/* These global variables are used to pass information between
   translate_addresses and find_address_in_section.  */
static bfd_vma pc;
static const char *filename;
static const char *functionname;
static unsigned int line;
static bfd_boolean found;



/* Look for an address in a section.  This is called via
   bfd_map_over_sections.*/
static void find_address_in_section(bfd *abfd, asection *section, PTR data ATTRIBUTE_UNUSED)
{
  bfd_vma vma;
  bfd_size_type size;

  if (found)
    return;

  if ((bfd_get_section_flags (abfd, section) & SEC_ALLOC) == 0)
    return;

  vma = bfd_get_section_vma (abfd, section);
  if (pc < vma)
    return;

  size = bfd_get_section_size_before_reloc (section);
  if (pc >= vma + size)
    return;

  found = bfd_find_nearest_line (abfd, section, syms, pc - vma, &filename, &functionname, &line);
}




/* Read hexadecimal addresses from stdin, translate into
   file_name:line_number and optionally function name.  */
static void translate_addresses(bfd *abfd)
{
  int read_stdin = (naddr == 0);

  for (;;)
  {
      if (read_stdin)
	{
	  char addr_hex[100];

	  if (fgets (addr_hex, sizeof addr_hex, stdin) == 0)
	    break;
	  pc = bfd_scan_vma (addr_hex, 0, 16);
	}
      else
	{
	  if (naddr <= 0)
	    break;
	  --naddr;
	  pc = bfd_scan_vma (*addr++, 0, 16);
	}

      found = false;
      bfd_map_over_sections(abfd, find_address_in_section, (PTR)0);

      if (! found)
	{
	  if (with_functions)
	    printf ("??\n");
	  printf ("??:0\n");
	}
      else
	{
	  if (with_functions)
	    {
	      const char *name;
	      char *alloc = 0;

	      name = functionname;
	      if (name == 0 || *name == '\0')
		name = "??";
	      else if (do_demangle)
		{
		  alloc = demangle (abfd, name);
		  name = alloc;
		}

	      printf ("%s\n", name);

	      if (alloc != 0)
		free (alloc);
	    }

	  if (base_names && filename != 0)
	    {
	      char *h;

	      h = strrchr (filename, '/');
	      if (h != 0)
		filename = h + 1;
	    }

	  printf ("%s:%u\n", filename ? filename : "??", line);
	}

      /* fflush() is essential for using this command as a server
         child process that reads addresses from a pipe and responds
         with line number information, processing one address at a
         time.  */
      fflush (stdout);
    }
}


/* Process a file.  */
static void process_file(const char *file_name, const char *target)
{
  bfd *abfd;
  char **matching;

  abfd = bfd_openr (file_name, target);
  if (abfd == 0)
    bfd_fatal (file_name);

  if (bfd_check_format (abfd, bfd_archive))
    std::cerr << "%s: can not get addresses from archive" << file_name << std::endl;

  if (! bfd_check_format_matches (abfd, bfd_object, &matching))
    {
      bfd_nonfatal (bfd_get_filename (abfd));
      if (bfd_get_error () == bfd_error_file_ambiguously_recognized)
	{
	  list_matching_formats (matching);
	  free (matching);
	}
      xexit (1);
    }

  slurp_symtab (abfd);

  translate_addresses (abfd);

  if (syms != 0)
    {
      free (syms);
      syms = 0;
    }

  bfd_close (abfd);
}


int main(int argc, char **argv)
{
  const char *file_name;
  char *target;
  int c;

#if defined (HAVE_SETLOCALE) && defined (HAVE_LC_MESSAGES)
  setlocale (LC_MESSAGES, "");
#endif
#if defined (HAVE_SETLOCALE)
  setlocale (LC_CTYPE, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  program_name = *argv;

  bfd_init();
  set_default_bfd_target();

  file_name =  optarg;;
  target = 0;

  if (file_name == 0)
    file_name = "a.out";

  addr = argv + optind;
  naddr = argc - optind;

  process_file (file_name, target);

  return 0;
}
