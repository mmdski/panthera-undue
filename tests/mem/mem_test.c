extern void
test_coarray(void);

extern void
test_crosssection(void);

extern void
test_list(void);

extern void
test_subsection(void);

int
main(void)
{
    test_coarray();
    test_list();
    test_subsection();
    test_crosssection();

    return 0;
}
