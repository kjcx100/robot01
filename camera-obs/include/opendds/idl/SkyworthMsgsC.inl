// -*- C++ -*-
// $Id$

/**
 * Code generated by the The ACE ORB (TAO) IDL Compiler v2.2a_p15
 * TAO and the TAO IDL Compiler have been developed by:
 *       Center for Distributed Object Computing
 *       Washington University
 *       St. Louis, MO
 *       USA
 *       http://www.cs.wustl.edu/~schmidt/doc-center.html
 * and
 *       Distributed Object Computing Laboratory
 *       University of California at Irvine
 *       Irvine, CA
 *       USA
 * and
 *       Institute for Software Integrated Systems
 *       Vanderbilt University
 *       Nashville, TN
 *       USA
 *       http://www.isis.vanderbilt.edu/
 *
 * Information about TAO is available at:
 *     http://www.cs.wustl.edu/~schmidt/TAO.html
 **/


// TAO_IDL - Generated from
// be/be_visitor_array/array_ci.cpp:150

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::EdgePolorPointList_forany>::free (
    SkyworthMsgs::EdgePolorPointList_slice * _tao_slice
  )
{
  SkyworthMsgs::EdgePolorPointList_free (_tao_slice);
}

ACE_INLINE
SkyworthMsgs::EdgePolorPointList_slice *
TAO::Array_Traits<SkyworthMsgs::EdgePolorPointList_forany>::dup (
    const SkyworthMsgs::EdgePolorPointList_slice * _tao_slice
  )
{
  return SkyworthMsgs::EdgePolorPointList_dup (_tao_slice);
}

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::EdgePolorPointList_forany>::copy (
    SkyworthMsgs::EdgePolorPointList_slice * _tao_to,
    const SkyworthMsgs::EdgePolorPointList_slice * _tao_from
  )
{
  SkyworthMsgs::EdgePolorPointList_copy (_tao_to, _tao_from);
}

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::EdgePolorPointList_forany>::zero (
    SkyworthMsgs::EdgePolorPointList_slice * _tao_slice
  )
{
  // Zero each individual element.
  for ( ::CORBA::ULong i0 = 0; i0 < 360; ++i0)
    {
      _tao_slice[i0] = SkyworthMsgs::EdgePolorPoint ();
    }
}

ACE_INLINE
SkyworthMsgs::EdgePolorPointList_slice *
TAO::Array_Traits<SkyworthMsgs::EdgePolorPointList_forany>::alloc (void)
{
  return SkyworthMsgs::EdgePolorPointList_alloc ();
}


// TAO_IDL - Generated from
// be/be_visitor_array/array_ci.cpp:150

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::RgbPicData_forany>::free (
    SkyworthMsgs::RgbPicData_slice * _tao_slice
  )
{
  SkyworthMsgs::RgbPicData_free (_tao_slice);
}

ACE_INLINE
SkyworthMsgs::RgbPicData_slice *
TAO::Array_Traits<SkyworthMsgs::RgbPicData_forany>::dup (
    const SkyworthMsgs::RgbPicData_slice * _tao_slice
  )
{
  return SkyworthMsgs::RgbPicData_dup (_tao_slice);
}

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::RgbPicData_forany>::copy (
    SkyworthMsgs::RgbPicData_slice * _tao_to,
    const SkyworthMsgs::RgbPicData_slice * _tao_from
  )
{
  SkyworthMsgs::RgbPicData_copy (_tao_to, _tao_from);
}

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::RgbPicData_forany>::zero (
    SkyworthMsgs::RgbPicData_slice * _tao_slice
  )
{
  // Zero each individual element.
  for ( ::CORBA::ULong i0 = 0; i0 < 230400; ++i0)
    {
      _tao_slice[i0] = ::CORBA::Char ();
    }
}

ACE_INLINE
SkyworthMsgs::RgbPicData_slice *
TAO::Array_Traits<SkyworthMsgs::RgbPicData_forany>::alloc (void)
{
  return SkyworthMsgs::RgbPicData_alloc ();
}


// TAO_IDL - Generated from
// be/be_visitor_array/array_ci.cpp:150

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::EdgeXyzPointList_forany>::free (
    SkyworthMsgs::EdgeXyzPointList_slice * _tao_slice
  )
{
  SkyworthMsgs::EdgeXyzPointList_free (_tao_slice);
}

ACE_INLINE
SkyworthMsgs::EdgeXyzPointList_slice *
TAO::Array_Traits<SkyworthMsgs::EdgeXyzPointList_forany>::dup (
    const SkyworthMsgs::EdgeXyzPointList_slice * _tao_slice
  )
{
  return SkyworthMsgs::EdgeXyzPointList_dup (_tao_slice);
}

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::EdgeXyzPointList_forany>::copy (
    SkyworthMsgs::EdgeXyzPointList_slice * _tao_to,
    const SkyworthMsgs::EdgeXyzPointList_slice * _tao_from
  )
{
  SkyworthMsgs::EdgeXyzPointList_copy (_tao_to, _tao_from);
}

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::EdgeXyzPointList_forany>::zero (
    SkyworthMsgs::EdgeXyzPointList_slice * _tao_slice
  )
{
  // Zero each individual element.
  for ( ::CORBA::ULong i0 = 0; i0 < 360; ++i0)
    {
      _tao_slice[i0] = SkyworthMsgs::EdgeXyzPoint ();
    }
}

ACE_INLINE
SkyworthMsgs::EdgeXyzPointList_slice *
TAO::Array_Traits<SkyworthMsgs::EdgeXyzPointList_forany>::alloc (void)
{
  return SkyworthMsgs::EdgeXyzPointList_alloc ();
}


// TAO_IDL - Generated from
// be/be_visitor_array/array_ci.cpp:150

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::MapPicData_forany>::free (
    SkyworthMsgs::MapPicData_slice * _tao_slice
  )
{
  SkyworthMsgs::MapPicData_free (_tao_slice);
}

ACE_INLINE
SkyworthMsgs::MapPicData_slice *
TAO::Array_Traits<SkyworthMsgs::MapPicData_forany>::dup (
    const SkyworthMsgs::MapPicData_slice * _tao_slice
  )
{
  return SkyworthMsgs::MapPicData_dup (_tao_slice);
}

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::MapPicData_forany>::copy (
    SkyworthMsgs::MapPicData_slice * _tao_to,
    const SkyworthMsgs::MapPicData_slice * _tao_from
  )
{
  SkyworthMsgs::MapPicData_copy (_tao_to, _tao_from);
}

ACE_INLINE
void
TAO::Array_Traits<SkyworthMsgs::MapPicData_forany>::zero (
    SkyworthMsgs::MapPicData_slice * _tao_slice
  )
{
  // Zero each individual element.
  for ( ::CORBA::ULong i0 = 0; i0 < 230400; ++i0)
    {
      _tao_slice[i0] = ::CORBA::Char ();
    }
}

ACE_INLINE
SkyworthMsgs::MapPicData_slice *
TAO::Array_Traits<SkyworthMsgs::MapPicData_forany>::alloc (void)
{
  return SkyworthMsgs::MapPicData_alloc ();
}


