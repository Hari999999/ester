
c**********************************************************************

	SUBROUTINE pertm_msol(dt)

c routine private du module mod_static

c routine d'interpolation m(t+dt)--->m(t) en tenant compte 	
c de la perte de masse (mdot > 0 : gain de masse,
c mdot < 0 : perte de masse)
c la perte de masse est suppos�e concentr�e dans la couche nm-1 nm
c on assure la stricte croissance des masses et on calcule la nouvelle
c masse totale sans d�passer msol

c utilisation par sbsp1dn (m**2/3 ---> m**2/3 ancien)

c Auteur: P.Morel, D�partement J.D. Cassini, O.C.A., CESAM2k


c en_masse = .true. variables lagrangiennes m23=m**23, r2=r**2
c en_masse = .false. variables eul�riennes m23=m, r2=r

c entr�es
c	bp,q,n,qt,knot,chim,mc,nc,mct,knotc : modele au temps t
c	dt : pas temporel

c entr�e/sortie
c	mstar : masse totale avec perte de masse

c sorties
c	old_ptm,x_ptm,xt_ptm,n_ptm,m_ptm,knot_ptm : interp. de
c	l'ancienne masse
c	en fonction de la nouvelle (en m**2/3) normalise (Mstar x Msol)
c	old_ptm et x_ptm sont identiques ce n'est pas le cas si
c	on tient compte de la perte de masse due a E=mc**2

c----------------------------------------------------------------

	USE mod_donnees, ONLY : langue, mdot, m_ptm, nchim, ne, ord_qs
	USE mod_kind
	USE mod_nuc, ONLY : l_planet, l_vent, planetoides
	USE mod_numerique, ONLY : bsp1dn, no_croiss
	USE mod_variables, ONLY : age, bp, knot, knot_ptm, mstar, mstar_t,
	1 n_ptm, n_qs, old_ptm, q, qt, xt_ptm, x_ptm
	
	IMPLICIT NONE
	
	REAL (kind=dp), DIMENSION(ne) :: df, f
	REAL (kind=dp), INTENT(in) :: dt	

	REAL (kind=dp), DIMENSION(n_qs) :: tmp1, tmp2
	REAL (kind=dp) :: m_planet, dm
	
	INTEGER :: i, l

c--------------------------------------------------------------------------
	
2000	FORMAT(8es10.3)

c extraction des masses au temps t+dt
c m^2/3 en lagrangien, m en eul�rien on a n_ptm=n_qs
	DO i=1,n_qs
	 CALL bsp1dn(ne,bp,q,qt,n_qs,ord_qs,knot,.TRUE.,q(i),l,f,df)
	 tmp1(i)=f(5) 		!; WRITE(*,2000)f	 
	ENDDO
	
c on s'assure de la stricte croissance	
	n_ptm=1 ; tmp2(n_ptm)=tmp1(1)
	DO i=2,n_qs
	 IF(tmp1(i) > tmp2(n_ptm))THEN
	  n_ptm=n_ptm+1 ; tmp2(n_ptm)=tmp1(i)
	 ENDIF
	ENDDO
	  
c allocations
	IF(ALLOCATED(old_ptm))DEALLOCATE(old_ptm,x_ptm,xt_ptm)
	ALLOCATE(old_ptm(1,n_ptm),x_ptm(n_ptm),xt_ptm(n_ptm+m_ptm))
	x_ptm=tmp2(1:n_ptm) ; old_ptm(1,1:n_ptm)=tmp2(1:n_ptm)
	
c tabulation de l'ancienne r�partition en fonction de la nouvelle
	CALL bsp1dn(1,old_ptm,x_ptm,xt_ptm,n_ptm,m_ptm,knot_ptm,.FALSE.,
	1 x_ptm(1),l,f,df)

c dm : perte/gain de masse, selon signe de mdot en Msol/an
	dm=mdot*1.d6*dt
	
c contribution des plan�to�des, m_planet en Msol/My
	IF(l_planet)THEN
	 CALL planetoides(m_planet=m_planet)
	 dm=dm+m_planet*dt
	ENDIF	
	
c calcul de mstar(t+dt), en entr�e mstar est la masse au temps t
c selon perte ou gain de masse on ne peut franchir Msol et on arr�te le
c vent d�s que mstar=Msol
	IF(mstar /= 1.d0)THEN
	
c contribution de la perte/gain de masse, mdot Msol/an	
	 dm=mdot*1.d6*dt
	 
c contribution des plan�to�des, m_planet Msol/My	 
	 IF(l_planet)THEN
	  CALL planetoides(m_planet=m_planet)
	  dm=dm+m_planet*dt
	 ENDIF	

c on ne peut d�passer 1Msol	
	 IF(dm > 0.d0)THEN
	  mstar=MIN(1.d0,mstar_t+dm)
	 ELSEIF(dm < 0.d0)THEN
	  mstar=MAX(1.d0,mstar_t+dm)
	 ENDIF
	 
	ELSE 	 
	 
	 
	 
	 IF(mstar == 1.d0)THEN
	  mdot=0.d0 ; l_vent=.FALSE.
	  SELECT CASE(langue)
	  CASE('english')
	   WRITE(*,1001)mstar ; WRITE(2,1001)mstar
1001	   FORMAT('the solar mass is reached at age =',es10.3,
	1  ', mdot is set to 0 for subsequent evoltion',/)
	  CASE DEFAULT
	   WRITE(*,1)age ; WRITE(2,1)age
1	   FORMAT('la masse solaire est atteinte pour age =',es10.3,
	1  ', on fixe mdot � 0 pour la suite',/)
	  END SELECT
	 ENDIF
	 
c contribution des plan�to�des, m_planet en Msol/My
	 IF(l_planet)THEN
	  CALL planetoides(m_planet=m_planet)	  
	  mstar=mstar+m_planet*dt	  
	 ENDIF	 
	ENDIF	
	
	RETURN

	END SUBROUTINE pertm_msol
