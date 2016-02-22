    module eos_davis_products_module

      use define_kind

      implicit none
      save

      public eos_davis_products_inv_t

      ! local subroutines
      private set_params
      private FindCJ
      private sonic         ! function
      !
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
                        Vc,     & ! specific volume
                        Pc,     & ! pressure
                        Tc,     & ! temperature
                        a,      &
                        b,      &
                        n,      &
                        k,      &
                        Cv        ! specific heat
      ! derived quantities
      real(REAL64) ::   V_0
      real(REAL64) ::   ec
      real(REAL64) ::   de        ! energy offset from CJ state

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
        real(REAL64)    :: Vcj,Pcj, Dcj, ecj,ucj, Tcj
        real(REAL64)    :: P, e
        integer         :: stat, table    
        integer, save   :: table_number

! ----- code
        table  = eos_params(1)
        Pscale = eos_params(2)
        if( Pscale .le. ZERO ) Pscale = 1e10_REAL64
        ! rho_scale = 1 ! units g/cm^3
        escale = Pscale ! / rho_scale
        !
        rho_0  = eos_params(3)      ! units g/cm^3
        e_0    = eos_params(4)      ! units escale
        P_0    = eos_params(5)      ! units Pscale
        Vc     = eos_params(6)      ! units cm^3/g
        Pc     = eos_params(7)      ! units Pscale
        Tc     = eos_params(8)      ! units K
        a      = eos_params(9)
        b      = eos_params(10)
        n      = eos_params(11)
        k      = eos_params(12)
        Cv     = eos_params(13)     ! units escale/K
        !
        if( rho_0 .le. ZERO )   &
          call global_error('EOS_DAVIS_PRODUCTS: rho_0 < 0')
        if( Vc .le. ZERO )   &
          call global_error('EOS_DAVIS_PRODUCTS: Vc <= 0')
        if( Pc .le. ZERO )   &
          call global_error('EOS_DAVIS_PRODUCTS: Pc <= 0')
        if( a .le. ZERO )   &
          call global_error('EOS_DAVIS_PRODUCTS: a <= 0')
        if( b .le. ZERO .or. b .ge. ONE )   &
          call global_error('EOS_DAVIS_PRODUCTS: b <= 0 or 1 <= b')
        if( n .le. ZERO )   &
          call global_error('EOS_DAVIS_PRODUCTS: n <= 0')
        if( k .le. ONE )   &
          call global_error('EOS_DAVIS_PRODUCTS: k <= 1')
        if( Cv .le. ZERO )   &
          call global_error('EOS_DAVIS_PRODUCTS: Cv <= 0')
        !
        ! convert to code units
        !
        P_0  = Pscale*P_0
        Pc   = Pscale*Pc
        !
        e_0  = escale*e_0
        !
        Cv   = (escale*KELVIN_PER_EV)*Cv
        !
        ! derived quantities
        !
        V_0 = ONE/rho_0
        ec  = Pc*Vc / (k-ONE + a)
        if( Tc .le. ZERO ) then
            Tc  = Pc*Vc / (Cv * TWO**(a*b/n) *(k-ONE+a))
        else
            Tc = Tc/KELVIN_PER_EV
        end if
        if( table .eq. table_number ) return
        table_number = table   
        !
        ! set de based on Hugoniot jump condition for CJ state
        !
        call FindCJ(Vcj,Pcj,stat)
        if( stat < 0 ) then
            call global_error('EOS_DAVIS_PRODUCTS: FindCJ failed')
        else if( stat > 0 ) then
            write(*,*) 'EOS_DAVIS_PRODUCTS: WARNING, FindCJ did not converge'
        endif

        write(*,'(a,i6.6)') 'EOS_DAVIS_PRODUCTS: CJ state for ', table_number
        Dcj = V_0 * sqrt( (Pcj-P_0)/(V_0-Vcj) )
        ucj = (ONE-Vcj/V_0)*Dcj
        ecj = HALF*(P_0+Pcj)*(V_0-Vcj)
        Tcj = T_s(Vcj)

        write(*,'(4x,a,1p,e14.6,a,e14.6, a)') &
              "V = ", Vcj, " cm^3/g  rho = ", ONE/Vcj, " g/cm^3"
        write(*,1000) "P = ", Pcj/1e10, " GPa"
        write(*,1000) "D = ", Dcj/1e5,  " km/s"
        write(*,1000) "u = ", ucj/1e5,  " km/s"
        write(*,1000) "e = ", ecj/1e10, " MJ/kg"
        write(*,1000) "T = ", Tcj*KELVIN_PER_EV, " K"
  1000 format(4x,a,1p,e10.3,a)
        !
        de = ZERO
        e = e_s(Vcj)
        de = ecj - (e-e_0)
        write(*,'("e(Vcj), de ",1p,2e14.6)') e/1e10, de/1e10

        ! check
        !e = e_VT(Vcj,Tcj)
        !P = P_VT(Vcj,Tcj)
        !write(*,'("P,e at(Vcj,Tcj) ",1p,2e14.6)') P/1e10, e/1e10


      end subroutine set_params

! -----------------------------------------------------------------------------
      
      subroutine FindCJ(V,P,stat)

! ----- purpose
!         Solve sonic condition equation for Vcj and Pcj = P_s(Vcj)
!          -dP_s/dV(Vcj) = (Pcj-P0)/(V0-Vcj)
!                { -1, bounding interval failed
!         stat = {  0, found solution
!                {  1, iteration did not converge

! ----- calling arguments
        real(REAL64), intent(out)   :: V,P
        integer,      intent(out)   :: stat

! ----- local variables
        integer     :: i
        real(REAL64) :: f,V1,V2

! ----- code
        stat = -1
        P = ZERO
        V = 0.9*V_0  ! initial guess
        ! find bounding interval
        f = sonic(V)
        if( f <= ZERO ) then
            V1 = V
            do i=1,MAX_ITR
               V2 = HALF*(V1-V_0)
               f = sonic(V2)
               if( f > 0 ) then
                   V = ZERO
                   exit
               endif
               V1 = V2
            end do
        else
            V2 = V
            do i=1,MAX_ITR
               V1 = 0.95_REAL64 * V2
               f = sonic(V1)
               if( f < 0 ) then
                   V = ZERO
                   exit
               endif
               V2 = V1
            end do
        endif
        if( V .ne. ZERO ) return      ! no bounding interval

        stat = 1
        ! bisection iteration
        do i=1,MAX_ITR

           V = HALF*(V1+V2)
           f = sonic(V)
           if( abs(f) < REL_TOL ) then
             stat = 0
             exit
           endif
           if( f < ZERO ) then
               V1 = V
           else
               V2 = V
           endif

        end do

        P = P_s(V)

      end subroutine FindCJ

! -----------------------------------------------------------------------------

      real(REAL64) function sonic(V)

! ----- purpose
!       evaluate sonic(V) = (dP_s/dV)(V) * (V0-V)/(P_s(V)-P0)  + 1
!       Note, sonic(V) is normalized to be dimensionless
!                < 0 if V < Vcj and P(V) > Pcj
!       sonic(V) = 0 if V=Vcj
!                > 0 if V > Vcj and P(V) < Pcj
!      

! ----- calling arguments
        real(REAL64), intent(in)    :: V

! ----- local variables
        integer     :: i
        real(REAL64) :: Ps, dPs

! ----- code
        Ps  = P_s(V)
        dPs = dP_s(V)

        sonic = dPs * (V_0-V)/(Ps-P_0) + ONE

      end function sonic

! ------------------------------------------------------------------------------

      real(REAL64) function P_s(V)
! ----- purpose
!       evaluate pressure on reference isentrope

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr, vn, f

! ----- code
        vr = V/Vc
        vn = vr**n
        f = TWO*a /(ONE + vn**2)
        P_s = (HALF*(vn+ONE/vn))**(a/n)/vr**(k+a) *(k-ONE+f)/(k-ONE+a) * Pc

      end function P_s

! ------------------------------------------------------------------------------
    
      real(REAL64) function dP_s(V)
! ----- purpose
!       evaluate dP_s/dV

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr, vn, f,df, Ps

! ----- code
        vr = V/Vc
        vn = vr**n
         f = TWO*a /(ONE + vn**2)
        df = n*(f*vn)**2/a
        Ps = (HALF*(vn+ONE/vn))**(a/n)/vr**(k+a) *(k-ONE+f)/(k-ONE+a) * Pc

        dP_s = - (k + TWO*a/(vn**2+ONE)+ df/(k-ONE+f))*Ps/V

      end function dP_s

! ------------------------------------------------------------------------------
    
      real(REAL64) function e_s(V)
! ----- purpose
!       evaluate energy on reference isentrope

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr, vn

! ----- code
        vr = V/Vc
        vn = vr**n

       e_s = ec*(HALF*(vn+ONE/vn))**(a/n)/vr**(k-ONE+a) + de

      end function e_s

! ------------------------------------------------------------------------------

      real(REAL64) function Gamma(V)
! ----- purpose
!       evaluate Gruneisen coefficient

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr, vn

! ----- code
        vr = V/Vc
        vn = vr**n

        Gamma = k-ONE + (ONE-b)*TWO*a/(ONE+vn**2)

      end function Gamma

! ------------------------------------------------------------------------------

      real(REAL64) function dGammaV(V)
! ----- purpose
!       evaluate d(Gamma/V)/dV

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr, vn, f, GoV

! ----- code
        vr = V/Vc
        vn = vr**n
         f = TWO*a /(ONE + vn**2)
        GoV = (k-ONE +(ONE-b)*f)/V        ! Gamma(V)/V

        dGammaV = -GoV/V - (ONE-b)*n*(f*vn/V)**2/a

      end function dGammaV

! ------------------------------------------------------------------------------

      real(REAL64) function T_s(V)
! ----- purpose
!       evaluate temperature on reference isentrope

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr, vn

! ----- code
        vr = V/Vc
        vn = vr**n
        
        T_s = (HALF*(vn+ONE/vn))**(a*(ONE-b)/n)/vr**(k-ONE+a*(ONE-b)) *Tc

      end function T_s

! ------------------------------------------------------------------------------

      real(REAL64) function dT_s(V)
! ----- purpose
!       evaluate dT_s/dV = -Gamma(V) * T_s(V)/V

! ----- calling arguments
        real(REAL64), intent(in) :: V

! ----- local variables
        real(REAL64)    :: vr, vn,Ts, f,GoV

! ----- code
        vr = V/Vc
        vn = vr**n
        
        Ts = (HALF*(vn+ONE/vn))**(a*(ONE-b)/n)/vr**(k-ONE+a*(ONE-b)) *Tc
         f = TWO*a /(ONE + vn**2)
        GoV = (k-ONE +(ONE-b)*f)/V        ! Gamma(V)/V

       dT_s = - GoV*Ts
        
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

         P_VT = P_s(V) + GammaV * Cv*(T - Ts)

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

         e_VT = e_s(V) + Cv*(T - Ts)

      end function e_VT

! ------------------------------------------------------------------------------

      real(REAL64) function KT(V,T)
! ----- purpose
!       evaluate isothermal bulk modulus K_T = -V*dP(V,T)/dV

! ----- calling arguments
        real(REAL64), intent(in) :: V
        real(REAL64), intent(in) :: T

! ----- local variables
        real(REAL64)    :: vr, vn, f,df, Ps,dPs, GoV,dGov, Ts

! ----- code
        vr = V/Vc
        vn = vr**n

         f = TWO*a /(ONE + vn**2)
        df = n*(f*vn)**2/a
        Ps = (HALF*(vn+ONE/vn))**(a/n)/vr**(k+a) *(k-ONE+f)/(k-ONE+a) * Pc
        dPs = - (k + TWO*a/(vn**2+ONE)+ df/(k-ONE+f))*Ps/V

        GoV = (k-ONE +(ONE-b)*f)/V                  ! Gamma(V)/V
        dGoV = -GoV/V - (ONE-b)*n*(f*vn/V)**2/a     ! d(Gama/V)/dV
        Ts = (HALF*(vn+ONE/vn))**(a*(ONE-b)/n)/vr**(k-ONE+a*(ONE-b)) *Tc

       KT = - (dPs + (dGoV*(T-Ts) + GoV**2*Ts)*Cv) * V

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
        if( KT(V,T) <= ZERO ) then
            NotDomain = .true.
        else
            NotDomain = .false.
        endif

      end function NotDomain

! -----------------------------------------------------------------------------
    
      subroutine eos_davis_products_inv_t(eos_params, tev, nump,pres, vol,sie)

        use library_module,   only : global_error
        use constants_module, only : KELVIN_PER_EV

! ----- purpose
!       return arrays of specific volumes and specific internal energy for an
!       isotherm from Davis products equation of state.

! ----- calling arguments
        real(REAL64), intent(in)               :: tev
        integer,      intent(in)               :: nump
        real(REAL64), intent(in), dimension(:) :: eos_params
        real(REAL64), intent(in)               :: pres(*)
        real(REAL64), intent(out)              :: vol(*)
        real(REAL64), intent(out)              :: sie(*)

! ----- local variables
        integer         :: i, stat
        real(REAL64)    :: P0, V1,P1, V2,P2, V,P
        ! bounds on isotherm
        real(REAL64)    :: Pmax, Vmin,emax
        real(REAL64)    :: Pmin, Vmax,emin

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
        !
        ! DEBUG
        !write(*,'("T ",1p,e14.6, " Pmin,Vmax ", 2e14.6, " Pmax,Vmin ", 2e14.6)') &
        !    tev*KELVIN_PER_EV, Pmin/1e10, Vmax, Pmax/1e10, Vmin
        !write(*,'("  de = ",1p,e14.6)' ) de/1e10
        !write(*,'(4x,3a14)') '  P(GPa)  ', '  V  ',  '  e(MJ/kg)  '
        !
        !
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
           ! DEBUG
           !write(*,'(4x,1p,3e14.6)') pres(i)/1e10, vol(i), sie(i)/1e10

        enddo ! l

 1000   format("***EOS_DAVIS_PRODUCTS*** ", a,1p, &
               " P,T",2(e15.6)," V1 V2",2(e15.6), " Plast", e15.6 )

      end subroutine eos_davis_products_inv_t

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
    
    end module eos_davis_products_module
