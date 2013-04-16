/*=========================================================================

  Library: QuickTools

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// ITK includes
#include <itkImageFileWriter.h>
#include <itkImageRegionIterator.h>
#include <itkPluginUtilities.h>
#include <itkVectorImage.h>

// QuickTools includes
#include "ImageMakerCLP.h"

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace
{

template <unsigned int D>
int DoIt( int argc, char * argv[], int numberOfComponents,
          const std::string& scalarType);

template <class OutputImageType, class OutputScalarType> int DoIt( int argc, char * argv[] );
} // end of anonymous namespace

//----------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  try
    {
    switch(Dimension)
      {
      case 1:
        return DoIt<1>(argc, argv, NumberOfComponents, ScalarType);
        break;
      case 2:
        return DoIt<2>(argc, argv, NumberOfComponents, ScalarType);
        break;
      default:
      case 3:
        return DoIt<3>(argc, argv, NumberOfComponents, ScalarType);
        break;
      }
    }

  catch( itk::ExceptionObject & excep )
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}


// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace
{

//----------------------------------------------------------------------------
template <unsigned int D>
int DoIt( int argc, char * argv[], int numberOfComponents, const std::string& scalarType)
{
  // Don't parse the arguments again, it's costly.

  const bool isVector = (numberOfComponents> 1);
  switch( itk::ImageIOBase::GetComponentTypeFromString(scalarType) )
    {
    case itk::ImageIOBase::UCHAR:
      return isVector ? DoIt<itk::VectorImage<unsigned char, D>, unsigned char >( argc, argv ) :
                        DoIt<itk::Image<unsigned char, D>, unsigned char >( argc, argv );
      break;
    case itk::ImageIOBase::CHAR:
      return isVector ? DoIt<itk::VectorImage<char, D>, char >( argc, argv ) :
                        DoIt<itk::Image<char, D>, char >( argc, argv );
      break;
    case itk::ImageIOBase::USHORT:
      return isVector ? DoIt<itk::VectorImage<unsigned short, D>, unsigned short >( argc, argv ) :
                        DoIt<itk::Image<unsigned short, D>, unsigned short>( argc, argv );
      break;
    case itk::ImageIOBase::SHORT:
      return isVector ? DoIt<itk::VectorImage<short, D>, short >( argc, argv ) :
                        DoIt<itk::Image<short, D>, short >( argc, argv );
      break;
    case itk::ImageIOBase::UINT:
      return isVector ? DoIt<itk::VectorImage<unsigned int, D>, unsigned int>( argc, argv ) :
                        DoIt<itk::Image<unsigned int, D>, unsigned int >( argc, argv );
      break;
    case itk::ImageIOBase::INT:
      return isVector ? DoIt<itk::VectorImage<int, D>, int >( argc, argv ) :
                        DoIt<itk::Image<int, D>, int >( argc, argv );
      break;
    case itk::ImageIOBase::ULONG:
      return isVector ? DoIt<itk::VectorImage<unsigned long, D>, unsigned long >( argc, argv ) :
                        DoIt<itk::Image<unsigned long, D>, unsigned long >( argc, argv );
      break;
    case itk::ImageIOBase::LONG:
      return isVector ? DoIt<itk::VectorImage<long, D>, long >( argc, argv ) :
                        DoIt<itk::Image<long, D>, long >( argc, argv );
      break;
    case itk::ImageIOBase::FLOAT:
      return isVector ? DoIt<itk::VectorImage<float, D>, float >( argc, argv ) :
                        DoIt<itk::Image<float, D>, float >( argc, argv );
      break;
    case itk::ImageIOBase::DOUBLE:
      return isVector ? DoIt<itk::VectorImage<double, D>, double >( argc, argv ) :
                        DoIt<itk::Image<double, D>, double >( argc, argv );
      break;
    case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
    default:
      std::cout << "unknown component type" << std::endl;
      break;
    }
  return EXIT_FAILURE;
}

//----------------------------------------------------------------------------
template <class OutputImageType, class OutputScalarType>
int DoIt( int argc, char * argv[] )
{
  PARSE_ARGS;

  typedef itk::ImageFileWriter<OutputImageType> WriterType;

  typename OutputImageType::Pointer image = OutputImageType::New();
  if (NumberOfComponents > 1)
    {
    image->SetNumberOfComponentsPerPixel( NumberOfComponents );
    }

  typename OutputImageType::SizeType size;
  for (int i = 0; i < OutputImageType::ImageDimension; ++i)
    {
    size[i] = Size[i];
    }
  typename OutputImageType::IndexType start;
  for (int i = 0; i < OutputImageType::ImageDimension; ++i)
    {
    start[i] = 0;
    }
  typename OutputImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  image->SetRegions( region );
  image->Allocate();

  double spacing[OutputImageType::ImageDimension];
  for (int i = 0; i < OutputImageType::ImageDimension; ++i)
    {
    spacing[i] = Spacing[i];
    }
  image->SetSpacing( spacing );

  double origin[OutputImageType::ImageDimension];
  for (int i = 0; i < OutputImageType::ImageDimension; ++i)
    {
    origin[i] = Origin[i];
    }
  image->SetOrigin( origin );

  typename OutputImageType::DirectionType direction;
  for ( unsigned int r = 0; r < OutputImageType::ImageDimension; r++ )
    {
    for ( unsigned int c = 0; c < OutputImageType::ImageDimension; c++ )
      {
      direction[r][c] = Direction[r*OutputImageType::ImageDimension+c];
      }
    }
  image->SetDirection( direction );

  // Set first index pixel value
  OutputScalarType* pixel = image->GetBufferPointer();
  for ( unsigned int c = 0; c < image->GetNumberOfComponentsPerPixel(); c++ )
    {
    pixel[c] = FillValue[c % FillValue.size()];
    }
  OutputImageType::IndexType firstIndex;
  for (int i = 0; i < OutputImageType::ImageDimension; ++i)
    {
    firstIndex[i] = 0;
    }
  image->FillBuffer(image->GetPixel(firstIndex));

  typename WriterType::Pointer writer = WriterType::New();
  itk::PluginFilterWatcher watchWriter(writer,
                                       "Write Volume",
                                       CLPProcessInformation);
  writer->SetFileName( OutputVolume.c_str() );
  writer->SetInput( image );
  //writer->SetUseCompression(1);
  writer->Update();

  return EXIT_SUCCESS;
}

} // end of anonymous namespace
