from xhtml2pdf import pisa
import io

def convert_html_to_pdf(html_text, output_filename):
    # Create a PDF file buffer
    pdf_buffer = io.BytesIO()
    
    # Convert HTML to PDF using xhtml2pdf
    pisa_status = pisa.CreatePDF(io.StringIO(html_text), dest=pdf_buffer)
    
    # Save the PDF file
    if not pisa_status.err:
        with open(output_filename, 'wb') as f:
            f.write(pdf_buffer.getvalue())
        print(f"PDF generated successfully: {output_filename}")
    else:
        print("Error in PDF generation")

# Example usage
html_text = "<style>@page{size: 58mm 100mm;}</style><h1>Hello, World!</h1><p>This is a test PDF document.</p>"
output_filename = "output.pdf"
convert_html_to_pdf(html_text, output_filename)