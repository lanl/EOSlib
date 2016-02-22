sub amrita2
{
  &amrita::get::token("Build::bin",*bin);
  &amrita::get::token("Build::lib",*lib);
  &amrita::get::token("Build::EOSlib",*EOSlib);
  &amrita::get::token("amrita::AMRBIN",*AMRBIN);
  &amrita::get::token("amrita::AMRSO",*AMRSO);
}
1;
