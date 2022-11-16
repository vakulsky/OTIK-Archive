//
// Created by grigory on 16/11/22.
//

#include "ReedSolomonWrapper.h"


void ReedSolomonWrapper::AddProtection(const string& inFileName, const string& outFileName){


    const std::size_t field_descriptor    =   8;
    const std::size_t gen_poly_index      = 120;
    const std::size_t gen_poly_root_count =   6;
    const std::size_t code_length         = 255;
    const std::size_t fec_length          = 6;

    typedef schifra::reed_solomon::encoder<code_length,fec_length> encoder_t;
    typedef schifra::reed_solomon::file_encoder<code_length,fec_length> file_encoder_t;

    const schifra::galois::field field(field_descriptor,
                                       schifra::galois::primitive_polynomial_size06,
                                       schifra::galois::primitive_polynomial06);

    schifra::galois::field_polynomial generator_polynomial(field);

    if (
            !schifra::make_sequential_root_generator_polynomial(field,
                                                                gen_poly_index,
                                                                gen_poly_root_count,
                                                                generator_polynomial)
            )
    {
        std::cout << "Error - Failed to create sequential root generator!" << std::endl;

    }
    else{
        const encoder_t rs_encoder(field,generator_polynomial);

        file_encoder_t(rs_encoder, inFileName, outFileName);
    }

}
void ReedSolomonWrapper::RemoveProtection(const string& inFileName, const string& outFileName){
    const std::size_t field_descriptor    =   8;
    const std::size_t gen_poly_index      = 120;
    const std::size_t code_length         = 255;
    const std::size_t fec_length          =   6;

    typedef schifra::reed_solomon::decoder<code_length,fec_length> decoder_t;
    typedef schifra::reed_solomon::file_decoder<code_length,fec_length> file_decoder_t;

    const schifra::galois::field field(field_descriptor,
                                       schifra::galois::primitive_polynomial_size06,
                                       schifra::galois::primitive_polynomial06);

    const decoder_t rs_decoder(field,gen_poly_index);

    file_decoder_t(rs_decoder, inFileName, outFileName);
}
