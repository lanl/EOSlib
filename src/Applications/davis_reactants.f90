
    module eos_davis_reactants_module

      use define_kind

      implicit none
      save

      public eos_davis_reactants_inv_t

      ! local subroutines
      private set_params
      private FindInterval
      private refine

      ! functions of (V)
      private P_s
      private dP_s
      private T_s
      private dT_s
      private e_s
      private Gamma
      private dGammaV

      ! functions of (V,T)
      private P_VT
      private e_VT
      private NotDomain

      private

      ! local

      integer, parameter      :: MAX_ITR = 100
      ! tolerance for pressure
      real(REAL64), parameter :: REL_TOL = 1e-10_REAL64
      real(REAL64), parameter :: ABS_TOL = 1        ! dyne/cm^2 = microbar
      ! possibly set ABS_TOL = REL_TOL*Pscale ?
      ! possibly ABS_TOL and REL_TOl as input parameters ?

      ! model parameters
      real(REAL64) ::   Pscale, & ! pressure scale
                        escale, & ! energy scale
                        rho_0,  & ! initial density
                        e_0,    & ! initial energy
                        P_0,    & ! initial pressure
                        T_0,    & ! initial temperature
                        A,      &
                        B,      &
                        C,      &
                        Gamma0, &
                        Z,      &
                        alpha,  &
                        Cv0       ! specific heat
      ! derived quantities
      real(REAL64) ::   V_0, Phat, ehat

! ==============================================================================
    contains
! ------------------------------------------------------------------------------

      subroutine set_params(eos_params)

        use library_module,   only : global_error
        use constants_module, only : KELVIN_PER_EV

! ----- purpose
!       picks model params out of eos_params array

! ----- calling arguments

        real(REAL64), intent(in), dimension(:) :: eos_params

! ----- local variables

! ----- code
        ! eos_params(1) is table number
        Pscale = eos_params(2)
        if( Pscale .le. ZERO ) Pscale = 1e10_REAL64
        escale = Pscale
        !
        rho_0  = eos_params(3)      ! units g/cm^3
        e_0    = eos_params(4)      ! units escale
        P_0    = eos_params(5)      ! units Pscale
        T_0    = eos_params(6)      ! units K
        A      = eos_params(7)      ! units uscale = sqrt(escale)
        B      = eos_params(8)
        C      = eos_params(9)
        Gamma0 = eos_params(10)
        Z      = eos_params(11)
        alpha  = eos_params(12)
        Cv0    = eos_params(13)     ! units escale/K
        !
        if( rho_0 .le. ZERO )   &
          call global_error('EOS_DAVIS_REACTANTS: rho_0.le.ZERO')
        if( T_0 .le. ZERO )     T_0 = 296.15_REAL64
        if( A .le. ZERO )   &
          call global_error('EOS_DAVIS_REACTANTS: A.le.ZERO')
        if( B .le. ZERO )   &
          call global_error('EOS_DAVIS_REACTANTS: B.le.ZERO')
        if( C .le. ZERO )   &
          call global_error('EOS_DAVIS_REACTANTS: C.le.ZERO')
        if( Gamma0 .le. ZERO )   &
          call global_error('EOS_DAVIS_REACTANTS: Gamma0.le.ZERO')
        if( Z .le. ZERO )   &
          call global_error('EOS_DAVIS_REACTANTS: Z.le.ZERO')
        if( alpha .le. ZERO )   &
          call global_error('EOS_DAVIS_REACTANTS: alpha.le.ZERO')
        if( Cv0 .le. ZERO )   &
          call global_error('EOS_DAVIS_REACTANTS: Cv0.le.ZERO')
        !
        V_0  = ONE/rho_0
        Phat = QUARTER *rho_0*A**2/B
        ehat = Phat/rho_0
        !
        ! convert to code units
        !
        Phat = Pscale*Phat
        P_0  = Pscale*P_0
        !
        ehat = escale*ehat
        e_0  = escale*e_0
        !
        T_0  = T_0 / KELVIN_PER_EV
        Cv0  = (escale*KELVIN_PER_EV)*Cv0

      end subroutine set_params

! ------------------------------------------------------------------------------

      real(REAL64) function P_s(V)
! ----- purpose
!       evaluate pressure on reference isentrope

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: y, yb

! ----- code
         y = ONE - V*rho_0
        yb = FOUR*B*y
        P_s = P_0 + (yb*(ONE+HALF*yb*(ONE+THIRD*yb*(ONE+QUARTER*C*yb))) &
                      + (y/(V*rho_0)**2)**2) * Phat

      end function P_s

! ------------------------------------------------------------------------------
    
      real(REAL64) function dP_s(V)
! ----- purpose
!       evaluate dP_s/dV

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: y, yb

! ----- code
         y = ONE - V*rho_0
        yb = FOUR*B*y
        dP_s = - ( FOUR*B/V_0*(ONE+yb*(ONE+HALF*yb*(ONE+THIRD*C*yb))) &
                      + y*(V_0/V)**4 *(TWO/V_0 + FOUR*y/V)) * Phat

      end function dP_s

! ------------------------------------------------------------------------------
    
      real(REAL64) function e_s(V)
! ----- purpose
!       evaluate energy on reference isentrope

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr, y, yb

! ----- code
        vr = V*rho_0
         y = ONE - vr
        yb = FOUR*B*y
        e_s =  e_0 + P_0*(V_0-V) &
              + Phat*(HALF*y*yb*(ONE+THIRD*yb*(ONE+QUARTER*yb*(ONE+C*FIFTH*yb))) &
                       + vr*(ONE-vr*(ONE-THIRD*vr)) - THIRD);

      end function e_s

! ------------------------------------------------------------------------------

      real(REAL64) function Gamma(V)
! ----- purpose
!       evaluate Gruneisen coefficient

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- code
        Gamma = Gamma0 + Z*(ONE-V*rho_0)

      end function Gamma

! ------------------------------------------------------------------------------

      real(REAL64) function dGammaV(V)
! ----- purpose
!       evaluate d(Gamma/V)/dV

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- code
        dGammaV = -(Gamma0+Z)/V**2

      end function dGammaV

! ------------------------------------------------------------------------------

      real(REAL64) function T_s(V)
! ----- purpose
!       evaluate temperature on reference isentrope

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr

! ----- code
       vr = V*rho_0;
       T_s = T_0*exp(-Z*(ONE-vr)) / vr**(Gamma0+Z);
        
      end function T_s

! ------------------------------------------------------------------------------

      real(REAL64) function dT_s(V)
! ----- purpose
!       evaluate dT_s/dV = -Gamma(V) * T_s(V)/V

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr, Ts

! ----- code
       vr = V*rho_0;
       Ts = T_0*exp(-Z*(ONE-vr)) / vr**(Gamma0+Z)

       dT_s = (Z/V_0 - (Gamma0+Z)/V)*Ts
        
      end function dT_s

! ------------------------------------------------------------------------------

      real(REAL64) function P_VT(V,T)
! ----- purpose
!       evaluate pressure as function of V & T

! ----- calling arguments
        real(REAL64), intent(in) :: V
        real(REAL64), intent(in) :: T

! ----- local variables
        real(REAL64)    :: Ts, GammaV

! ----- code
         Ts = T_s(V)
         GammaV = Gamma(V)/V

         P_VT = P_s(V) + GammaV*((T/Ts)**(ONE+alpha)-ONE) * Cv0*Ts/(ONE+alpha)

      end function P_VT

! ------------------------------------------------------------------------------

      real(REAL64) function e_VT(V,T)
! ----- purpose
!       evaluate energy as function of V & T

! ----- calling arguments
        real(REAL64), intent(in) :: V
        real(REAL64), intent(in) :: T

! ----- local variables
        real(REAL64)    :: Ts

! ----- code
         Ts = T_s(V)

         e_VT = e_s(V) + ((T/Ts)**(ONE+alpha) - ONE) * Cv0*Ts/(ONE+alpha)

      end function e_VT

! ------------------------------------------------------------------------------

      real(REAL64) function KT(V,T)
! ----- purpose
!       evaluate isothermal bulk modulus K_T = -V*dP(V,T)/dV

! ----- calling arguments
        real(REAL64), intent(in) :: V
        real(REAL64), intent(in) :: T

! ----- local variables
        real(REAL64)    :: Ts,Talpha

! ----- code
        Ts = T_s(V)
        Talpha = (T/Ts)**(ONE+alpha)
        KT =  V*dGammaV(V)*(Talpha-ONE) + Gamma(V)**2/V *(alpha*Talpha+ONE)
        KT = - (V*dP_s(V) + Cv0*Ts*KT/(ONE+alpha))
      end function KT

! ------------------------------------------------------------------------------

      logical function NotDomain(V,T)
! ----- purpose
!       true if (V,T) not in domain of EOS model

! ----- calling arguments
        real(REAL64), intent(in) :: V
        real(REAL64), intent(in) :: T

! ----- local variables
        real(REAL64)    :: Ts,Talpha

! ----- code
        if( Gamma(V) <= ZERO ) then
            NotDomain = .true.
        else if( KT(V,T) <= ZERO ) then
            NotDomain = .true.
        else
            NotDomain = .false.
        endif

      end function NotDomain

! -----------------------------------------------------------------------------
    
      subroutine eos_davis_reactants_inv_t(eos_params, tev, nump,pres, vol,sie)

        use library_module,   only : global_error

! ----- purpose
!       return arrays of specific volumes and specific internal energy for an
!       isotherm from Davis reactants equation of state.

! ----- calling arguments
        real(REAL64), intent(in)               :: tev
        integer,      intent(in)               :: nump
        real(REAL64), intent(in), dimension(:) :: eos_params
        real(REAL64), intent(in)               :: pres(*)
        real(REAL64), intent(out)              :: vol(*)
        real(REAL64), intent(out)              :: sie(*)

! ----- local variables
        integer      :: i, stat
        real(REAL64) :: P0, V1,P1, V2,P2, V,P
        ! bounds on isotherm
        real(REAL64) :: Pmax, Vmin,emax
        real(REAL64) :: Pmin, Vmax,emin

! ----- code
        call set_params(eos_params)

        ! check if isotherm out of domain
        if( KT(V_0,tev) <= ZERO ) then
            vol(1:nump) = V_0
            sie(1:nump) = e_VT(V_0,tev)
            return
        endif

        ! find bounds for interval of isotherm connected to (V_0,tev)
        call FindInterval(tev,V_0,pres(1), V1,P1,V2,P2)
        Pmin = P1
        Vmax = V1
        emin = e_VT(Vmax,tev)
        !
        call FindInterval(tev,V_0,pres(nump), V1,P1,V2,P2)
        Pmax = P2
        Vmin = V2
        emax = e_VT(Vmin,tev)

        P0 = P_VT(V_0,tev)
        V  = Vmax    ! upper bound for interval
        do i=1,nump
           
           if( pres(i) >= Pmax ) then
               vol(i) = Vmin
               sie(i) = emax
           else if( pres(i) <= Pmin ) then
               vol(i) = Vmax
               sie(i) = emin
           else
               ! set interval for refine based on last V, V_0 or Vmin
               ! Alternative: save array of trials from FindInterval
               !              and use nearest pair
               if( pres(i) < P0 ) then
                   V1 = V_0
                   V2 = V
               else
                   V1 = Vmin
                   V2 = min(V_0,V)
               endif

               call refine(tev,pres(i), V1,V2, V,P, stat)
               if( stat > 0 ) then
                   write(*, 1000) 'failed to converge:', pres(i),tev, V1,V2, P
               else if( stat < 0 ) then
                   write(*, 1000) 'domain error:', pres(i),tev, V1,V2, P
               endif

               vol(i) = V
               sie(i) = e_VT(V,tev)
               if( stat < 0 ) V = V2   ! upper bound for next interval

           endif

        enddo ! l

 1000   format("***EOS_DAVIS_REACTANTS*** ", a,1p, &
               " P,T",2(e15.6)," V1 V2",2(e15.6), " Plast", e15.6 )

      end subroutine eos_davis_reactants_inv_t

! -----------------------------------------------------------------------------
      
      subroutine FindInterval(T,V,P, V1,P1,V2,P2)

! ----- purpose
!       find interval of isotherm closest to V that contains P or close to P
!       either
!          P1=P(V1,T) <= P <= P2=P(V2,T) if P in domain
!       or P2=P1< P and V2=V1            if compressive and P outside domain
!       or P <P1=P2 and V1=V2            if expansive   and P outside domain
!       expansive/compressive relative to input V
!       Domain restricted to connected interval with dP/dV < 0

! ----- calling arguments
        real(REAL64), intent(in)    :: T
        real(REAL64), intent(in)    :: V,P
        real(REAL64), intent(out)   :: V1,P1
        real(REAL64), intent(out)   :: V2,P2

! ----- local variables
        integer      :: i
        real(REAL64) :: f

! ----- code
        V1 = V
        P1 = P_VT(V,T)
        V2 = V1
        P2 = P1

        if( P == P1 ) return

        if( P > P1 ) then
        ! compressive
            f = 0.9_REAL64
            do i = 1, MAX_ITR
               V2 = f*V1
               if( NotDomain(V2,T) ) then
                   f = HALF*(ONE+f)
                   cycle
               end if
               P2 = P_VT(V2,T)
               if( P2 >= P ) return
               if( P2 < P1 ) then
                   f = HALF*(ONE+f)
                   cycle
               end if
               P1 = P2
               V1 = V2
            end do
            P2 = P1     ! P1=P2 < P outside domain
            V2 = V1

        else
        ! expansive
            f = 1.1_REAL64
            do i = 1, MAX_ITR
               V1 = f*V2
               if( NotDomain(V1,T) ) then
                   f = HALF*(ONE+f)
                   cycle
               end if
               P1 = P_VT(V1,T)
               if( P1 <= P ) return
               if( P1 > P2 ) then
                   f = HALF*(ONE+f)
                   cycle
               end if
               P2 = P1
               V2 = V1
            end do
            P1 = P2     ! P1=P2 > P outside domain
            V1 = V2
        end if

      end subroutine FindInterval

! -----------------------------------------------------------------------------
      
      subroutine refine(T,P, V1,V2, V,Pv, stat)

! ----- purpose
!       Newton/bisect iteration to refine interval (V1,V2)
!         P(V1,T) > P > P(V2,T) til abs(P - Pv) < tol where Pv = P(V,T)
!              { -1, domain error
!       stat = {  0, OK
!              {  1, failed to converge
!       Note: isotherm should be convex: dP/dV < 0 and d^2P/dV^2 > 0
!             and V2 should be good initial guess
!
! ----- calling arguments
        real(REAL64), intent(in)    :: T
        real(REAL64), intent(in)    :: P
        real(REAL64), intent(inout) :: V1
        real(REAL64), intent(inout) :: V2
        real(REAL64), intent(out)   :: V, Pv
        integer,      intent(out)   :: stat


! ----- local variables
        integer      :: i
        real(REAL64) :: tol
        real(REAL64) :: dP,K, dV

! ----- code
        tol = max(REL_TOL*P,ABS_TOL)
        stat = 1        ! tolerance not met
        V = V2          ! initial guess
        Pv = P_VT(V,T)
        dP = Pv - P

        do i=1,MAX_ITR

           K =  KT(V,T)     ! K = -V*dP/dV > 0
           if( K <= ZERO ) then
               stat = -1    ! domain error
               return
           end if

           ! Newton step
           V = (ONE + (dP/K))*V
           if( V<V1 .or. V2<V ) then
              ! outside interval, replace with bisection step
              V = HALF*(V1+V2)
           endif
           Pv = P_VT(V,T)
           dP = Pv - P

           ! shrink interval
           if( dP > ZERO ) then
               V1 = V
           else
               V2 = V
           end if

           ! check tolerance
           if( abs(dP) < tol ) then
               stat = 0
               exit
           endif

        end do  ! i

        if( stat == 0 ) then
            if( KT(V,T) <= ZERO ) stat = -1 ! domain error
        endif

      end subroutine refine

! ------------------------------------------------------------------------------
    
    end module eos_davis_reactants_module
! =============================================================================
