sub amrita3
{
  use Amrita::Interface;
  $bin    = $amrita->get_token("Build::bin");
  $lib    = $amrita->get_token("Build::lib");
  $EOSlib = $amrita->get_token("Build::EOSlib");
  $AMRBIN = $amrita->get_token("amrita::AMRBIN");
  $AMRSO  = $amrita->get_token("amrita::AMRSO");
}
1;
